#ifndef ASM_HPP
#define ASM_HPP

#include "ast.hpp"
#include "symtab.hpp"
#include "rtl.hpp"
#include <string>
#include <vector>
#include <set>
#include <map>

/* ════════════════════════════════════════════
   VARIABLE LOCATION TRACKER
   ════════════════════════════════════════════ */
static std::map<std::string, int> asm_local_offset;
static int asm_frame_size = 0;
static int asm_label_count = 0;
static int asm_stemp_cnt   = 0;

static char asm_current_func[128];
static char asm_ret_label[16];
static int  asm_stemp_offset = 0;
static int  asm_has_return = 0;
static int asm_ternary_offsets[64];
static int asm_ternary_cnt_prescan = 0;
static const char *asm_current_ret_type = "void";

static char *asm_new_label(void) {
    char *b = (char*)malloc(16);
    sprintf(b, "Label%d", asm_label_count++);
    return b;
}

static void asm_prescan_expr(ASTNode *node, int *neg_size) {
    if (!node) return;
    if (strcmp(node->label, "Condition: ?:") == 0) {
        asm_prescan_expr(node->child1, neg_size);
        int is_f = (node->child2 && strcmp(node_type(node->child2), "float") == 0);
        if (is_f) *neg_size += 8; else *neg_size += 4;
        asm_ternary_offsets[asm_ternary_cnt_prescan++] = -(*neg_size);
        asm_prescan_expr(node->child2, neg_size);
        asm_prescan_expr(node->child3, neg_size);
    } else if (strncmp(node->label, "FuncCall:", 9) == 0) {
        ASTNode *arg = node->child1;
        while (arg) {
            ASTNode *ns = arg->next; arg->next = NULL;
            asm_prescan_expr(arg, neg_size);
            arg->next = ns; arg = ns;
        }
    } else {
        asm_prescan_expr(node->child1, neg_size);
        asm_prescan_expr(node->child2, neg_size);
        asm_prescan_expr(node->child3, neg_size);
    }
}

static void asm_prescan_stmt(ASTNode *node, int *neg_size) {
    while (node) {
        const char *lbl = node->label;
        if (strcmp(lbl, "Asgn:") == 0) {
            asm_prescan_expr(node->child2, neg_size);
        } else if (strncmp(lbl, "Write:", 6) == 0 && node->child1) {
            asm_prescan_expr(node->child1, neg_size);
        } else if (strncmp(lbl, "Return:", 7) == 0 && node->child1) {
            asm_prescan_expr(node->child1, neg_size);
        } else if (strcmp(lbl, "If:") == 0) {
            asm_prescan_expr(node->child1, neg_size);
            asm_prescan_stmt(node->child2, neg_size);
            asm_prescan_stmt(node->child3, neg_size);
        } else if (strcmp(lbl, "While:") == 0) {
            asm_prescan_expr(node->child1, neg_size);
            asm_prescan_stmt(node->child2, neg_size);
        } else if (strcmp(lbl, "Do:") == 0) {
            asm_prescan_stmt(node->child1, neg_size);
            asm_prescan_expr(node->child2, neg_size);
        } else if (strncmp(lbl, "FuncCall:", 9) == 0) {
            ASTNode *arg = node->child1;
            while (arg) {
                ASTNode *ns = arg->next; arg->next = NULL;
                asm_prescan_expr(arg, neg_size);
                arg->next = ns; arg = ns;
            }
        }
        node = node->next;
    }
}

static void asm_collect_locals_for_func(FuncSymbol *fn) {
    asm_local_offset.clear();
    int param_offset = 8;
    for (int p = 0; p < fn->param_count; p++) {
        char buf[128]; sprintf(buf, "%s_", fn->param_names[p]);
        asm_local_offset[std::string(buf)] = param_offset;
        param_offset += (strcmp(fn->param_types[p], "float") == 0) ? 8 : 4;
    }
    int neg_size = 0;
    asm_stemp_offset = 0;
    if (strcmp(fn->ret_type, "void") != 0) {
        if (strcmp(fn->ret_type, "float") == 0) { neg_size += 8; asm_stemp_offset = -neg_size; }
        else { neg_size += 4; asm_stemp_offset = -neg_size; }
    }
    for (int i = 0; i < fn->local_sym_count; i++) {
        bool is_param = false;
        for (int p = 0; p < fn->param_count; p++)
            if (strcmp(fn->local_syms[i].name, fn->param_names[p]) == 0) { is_param = true; break; }
        if (is_param) continue;
        char buf[128]; sprintf(buf, "%s_", fn->local_syms[i].name);
        if (strcmp(fn->local_syms[i].type, "float") == 0) { neg_size += 8; asm_local_offset[std::string(buf)] = -neg_size; }
        else { neg_size += 4; asm_local_offset[std::string(buf)] = -neg_size; }
    }
    asm_ternary_cnt_prescan = 0;
    asm_prescan_stmt(fn->body, &neg_size);
    asm_frame_size = neg_size + 8;
    if (asm_frame_size < 8) asm_frame_size = 8;
}

static bool asm_is_global(const char *var) {
    auto it = asm_local_offset.find(std::string(var));
    if (it != asm_local_offset.end()) return false;
    char name[64]; strcpy(name, var);
    int len = strlen(name);
    if (len > 0 && name[len-1] == '_') name[len-1] = '\0';
    for (int i = 0; i < sym_count; i++)
        if (strcmp(sym_table[i].name, name) == 0) return true;
    return false;
}

static int asm_get_offset(const char *var) {
    auto it = asm_local_offset.find(std::string(var));
    if (it != asm_local_offset.end()) return it->second;
    return -8;
}

/* ════════════════════════════════════════════
   LOAD / STORE HELPERS  (MIPS instructions)
   ════════════════════════════════════════════ */
static void asm_load_var(const char *var, const char *reg, FILE *out) {
    if (asm_is_global(var)) fprintf(out, "\tlw $%s, %s\t\t\t# Source:%s\n", reg, var, var);
    else fprintf(out, "\tlw $%s, %d($fp)\t\t\t# Source:%s\n", reg, asm_get_offset(var), var);
}
static void asm_store_var(const char *var, const char *reg, FILE *out) {
    if (asm_is_global(var)) fprintf(out, "\tsw $%s, %s\t\t\t# Dest: $%s\n", reg, var, reg);
    else fprintf(out, "\tsw $%s, %d($fp)\t\t\t# Dest: $%s\n", reg, asm_get_offset(var), reg);
}
static void asm_load_float_var(const char *var, const char *freg, FILE *out) {
    if (asm_is_global(var)) fprintf(out, "\tl.d $%s, %s\t\t\t# Source:%s\n", freg, var, var);
    else fprintf(out, "\tl.d $%s, %d($fp)\t\t\t# Source:%s\n", freg, asm_get_offset(var), var);
}
static void asm_store_float_var(const char *var, const char *freg, FILE *out) {
    if (asm_is_global(var)) fprintf(out, "\ts.d $%s, %s\t\t\t# Dest: $%s\n", freg, var, freg);
    else fprintf(out, "\ts.d $%s, %d($fp)\t\t\t# Dest: $%s\n", freg, asm_get_offset(var), freg);
}

/* Load leaf node into a specific register — MIPS version of rtl_load_into */
static void asm_load_into(ASTNode *node, const char *reg, FILE *out) {
    const char *lbl = node->label; char buf[256];
    if (strncmp(lbl, "Name:", 5) == 0) {
        rtl_name(lbl, buf);
        if (strstr(lbl, "<float>")) asm_load_float_var(buf, reg, out);
        else asm_load_var(buf, reg, out);
    } else if (strncmp(lbl, "Num:", 4) == 0) {
        rtl_num(lbl, buf);
        if (strstr(lbl, "<float>")) { double v = atof(buf); fprintf(out, "\tli.d $%s, %.2f\t\t\t# Source:%.6f\n", reg, v, v); }
        else fprintf(out, "\tli $%s, %s\t\t\t# Source:%s\n", reg, buf, buf);
    } else if (strncmp(lbl, "Str:", 4) == 0) {
        char buf2[256]; rtl_str(lbl, buf2); int idx = rtl_add_string(buf2);
        char sn[32]; sprintf(sn, "_str_%d", idx);
        fprintf(out, "\tla $%s, %s\t\t\t# String = %s\n", reg, sn, buf2);
    } else if (strncmp(lbl, "Bool:", 5) == 0) {
        fprintf(out, "\tli $%s, %s\n", reg, strstr(lbl, "true") ? "1" : "0");
    }
}

/* Map AST label to MIPS instruction */
static const char *asm_mips_op(const char *lbl) {
    if (strstr(lbl,"Plus")) return "add"; if (strstr(lbl,"Minus")) return "sub";
    if (strstr(lbl,"Mult")) return "mul"; if (strstr(lbl,"Div")) return "div";
    if (strstr(lbl,": GT")) return "sgt"; if (strstr(lbl,": LT")) return "slt";
    if (strstr(lbl,": GE")) return "sge"; if (strstr(lbl,": LE")) return "sle";
    if (strstr(lbl,": EQ")) return "seq"; if (strstr(lbl,": NE")) return "sne";
    if (strstr(lbl,": AND")) return "and"; if (strstr(lbl,": OR")) return "or";
    return "?";
}
static const char *asm_mips_op_float(const char *lbl) {
    if (strstr(lbl,"Plus")) return "add.d"; if (strstr(lbl,"Minus")) return "sub.d";
    if (strstr(lbl,"Mult")) return "mul.d"; if (strstr(lbl,"Div")) return "div.d";
    return "?";
}

/* Emit MIPS binary op — handles div specially */
static void asm_emit_binop(const char *op, const char *res, const char *l, const char *r, FILE *out) {
    if (strcmp(op, "div") == 0) {
        fprintf(out, "\tdiv $%s, $%s, $%s\n", res, l, r);
    } else {
        fprintf(out, "\t%s $%s, $%s, $%s\t# Result: $%s, Opd1: $%s, Opd2:$%s\n",
                op, res, l, r, res, l, r);
    }
}

/* ════════════════════════════════════════════
   EXPRESSION GENERATOR — uses RTL register allocators
   Returns register name holding result.
   Mirrors rtl_gen_expr logic exactly.
   ════════════════════════════════════════════ */
static void asm_gen_stmt(ASTNode *node, FILE *out);

static const char *asm_gen_expr(ASTNode *node, FILE *out)
{
    if (!node) return "v0";
    const char *lbl = node->label;
    int is_float = (strcmp(node_type(node), "float") == 0);

    /* ── function call — before leaf check ── */
    if (strncmp(lbl, "FuncCall:", 9) == 0) {
        char fname[64]; const char *p = lbl + 10; const char *cut = strchr(p, '<');
        if (cut) { int n = cut-p; strncpy(fname, p, n); fname[n] = '\0'; } else strcpy(fname, p);
        int is_float_ret = (strcmp(node_type(node), "float") == 0);

        int argc = 0; ASTNode *args_arr[MAX_PARAMS]; ASTNode *arg = node->child1;
        while (arg) { args_arr[argc++] = arg; arg = arg->next; }

        if (argc > 0) {
    /* STEP 1: pre-compute non-leaf args in FORWARD order */
    const char *pre_regs[MAX_PARAMS] = {NULL};
    for (int i = 0; i < argc; i++) {
        ASTNode *ns = args_arr[i]->next; args_arr[i]->next = NULL;
        if (!is_leaf(args_arr[i]) || strncmp(args_arr[i]->label, "FuncCall:", 9) == 0) {
            pre_regs[i] = asm_gen_expr(args_arr[i], out);
        }
        args_arr[i]->next = ns;
        rtl_reset_regs();
    }

    /* STEP 2: push in REVERSE order */
    for (int i = argc - 1; i >= 0; i--) {
        ASTNode *ns = args_arr[i]->next; args_arr[i]->next = NULL;
        int af = (strcmp(node_type(args_arr[i]), "float") == 0);
        if (is_leaf(args_arr[i]) && strncmp(args_arr[i]->label, "FuncCall:", 9) != 0) {
            if (af) {
                asm_load_into(args_arr[i], "f2", out);
                fprintf(out, "\ts.d $f2, -4($sp)\t\t\t# store formal parameter at sp\n");
                fprintf(out, "\tsub $sp, $sp, 8\t\t\t# decrement the stack pointer by 8\n");
            } else {
                asm_load_into(args_arr[i], "v0", out);
                fprintf(out, "\tsw $v0, 0($sp)\t\t\t# store formal parameter at sp\n");
                fprintf(out, "\tsub $sp, $sp, 4\t\t\t# decrement the stack pointer by 4\n");
            }
        } else {
            const char *r = pre_regs[i];
            if (af) {
                fprintf(out, "\ts.d $%s, -4($sp)\t\t\t# store formal parameter at sp\n", r);
                fprintf(out, "\tsub $sp, $sp, 8\t\t\t# decrement the stack pointer by 8\n");
            } else {
                fprintf(out, "\tsw $%s, 0($sp)\t\t\t# store formal parameter at sp\n", r);
                fprintf(out, "\tsub $sp, $sp, 4\t\t\t# decrement the stack pointer by 4\n");
            }
        }
        args_arr[i]->next = ns;
    }
}

        fprintf(out, "\tjal %s_\n", fname);

        for (int i = 0; i < argc; i++) {
            int af = (strcmp(node_type(args_arr[i]), "float") == 0);
            fprintf(out, "\tadd $sp, $sp, %d\t\t\t# %s the stack pointer by %d\n",
                    af ? 8 : 4, af ? "increment" : "add", af ? 8 : 4);
        }

        return is_float_ret ? "f0" : "v1";
    }

    /* ── leaf ── */
    if (!node->child1 && !node->child2 && !node->child3) {
        if (is_float) {
            char *f = rtl_alloc_f_front();
            asm_load_into(node, f, out);
            return f;
        } else {
            char *reg = rtl_alloc_v0_or_t();
            asm_load_into(node, reg, out);
            return reg;
        }
    }

    /* ── unary minus ── */
    if (strncmp(lbl, "Arith: Uminus", 13) == 0) {
        const char *r = asm_gen_expr(node->child1, out);
        if (r[0] == 'f') {
            char *f = rtl_alloc_f_front();
            rtl_free_f(r);
            fprintf(out, "\tneg.d $%s, $%s\n", f, r);
            return f;
        } else {
            char *res;
            if (r[0] == 't' && !rtl_v0_live) res = rtl_get_v0();
            else res = rtl_alloc_t();
            rtl_free_reg(r);
            fprintf(out, "\tneg $%s, $%s\n", res, r);
            return res;
        }
    }

    /* ── logical NOT ── */
    if (strncmp(lbl, "Condition: NOT", 14) == 0) {
        const char *r = asm_gen_expr(node->child1, out);
        char *res;
        if (r[0] == 't' && !rtl_v0_live) res = rtl_get_v0();
        else res = rtl_alloc_t();
        rtl_free_reg(r);
        fprintf(out, "\txori $%s, $%s, 1\t# Result: $%s, Opd1: $%s\n", res, r, res, r);
        return res;
    }

    /* ── binary op ── */
    if (node->child1 && node->child2 && !node->child3) {
        bool llf = is_leaf(node->child1) && strncmp(node->child1->label, "FuncCall:", 9) != 0;
        bool rlf = is_leaf(node->child2) && strncmp(node->child2->label, "FuncCall:", 9) != 0;

        int child_is_float = node->child1 &&
                             (strcmp(node_type(node->child1), "float") == 0);

        /* ── float comparison ── */
        if (!is_float && child_is_float) {
            const char *f_l_reg = nullptr, *f_r_reg = nullptr;
            if (llf && rlf) {
                char *fl = rtl_alloc_f_front(); char *fr = rtl_alloc_f_front();
                asm_load_into(node->child1, fl, out);
                asm_load_into(node->child2, fr, out);
                f_l_reg = fl; f_r_reg = fr;
            } else if (!llf && rlf) {
                f_l_reg = asm_gen_expr(node->child1, out);
                char *fr = rtl_alloc_f_front();
                asm_load_into(node->child2, fr, out);
                f_r_reg = fr;
            } else if (llf && !rlf) {
                f_r_reg = asm_gen_expr(node->child2, out);
                char *fl = rtl_alloc_f_front();
                asm_load_into(node->child1, fl, out);
                f_l_reg = fl;
            } else {
                f_l_reg = asm_gen_expr(node->child1, out);
                f_r_reg = asm_gen_expr(node->child2, out);
            }

            /* emit MIPS float compare */
            if      (strstr(lbl, ": GT")) fprintf(out, "\tc.le.d $%s, $%s\n", f_l_reg, f_r_reg);
            else if (strstr(lbl, ": GE")) fprintf(out, "\tc.lt.d $%s, $%s\n", f_l_reg, f_r_reg);
            else if (strstr(lbl, ": LT")) fprintf(out, "\tc.lt.d $%s, $%s\n", f_l_reg, f_r_reg);
            else if (strstr(lbl, ": LE")) fprintf(out, "\tc.le.d $%s, $%s\n", f_l_reg, f_r_reg);
            else if (strstr(lbl, ": EQ")) fprintf(out, "\tc.eq.d $%s, $%s\n", f_l_reg, f_r_reg);
            else if (strstr(lbl, ": NE")) fprintf(out, "\tc.eq.d $%s, $%s\n", f_l_reg, f_r_reg);

            rtl_free_f(f_l_reg); rtl_free_f(f_r_reg);

            char *v = rtl_get_v0();
            char *t = rtl_alloc_t();
            bool is_direct = (strstr(lbl, ": LT") || strstr(lbl, ": LE") || strstr(lbl, ": EQ"));
            fprintf(out, "\tli $%s, 1\n", v);
            fprintf(out, "\tmove $%s, $zero\n", t);
            fprintf(out, "\t%s $%s, $%s, 0\n", is_direct ? "movt" : "movf", t, v);
            rtl_v0_live = false;
            return t;
        }

        /* ── float binary ── */
        if (is_float) {
            const char *op = asm_mips_op_float(lbl);
            const char *f_l_reg = nullptr, *f_r_reg = nullptr;

            if (llf && rlf) {
                char *fl = rtl_alloc_f_front();
                char *f_res = rtl_alloc_f_front();
                char *fr = rtl_alloc_f_front();
                asm_load_into(node->child1, fl, out);
                asm_load_into(node->child2, fr, out);
                rtl_free_f(fl); rtl_free_f(fr);
                fprintf(out, "\t%s $%s, $%s, $%s\t# Result: $%s, Opd1: $%s, Opd2:$%s\n",
                        op, f_res, fl, fr, f_res, fl, fr);
                return f_res;
            } else if (llf && !rlf) {
                f_r_reg = asm_gen_expr(node->child2, out);
                char *fl = rtl_alloc_f_front();
                asm_load_into(node->child1, fl, out);
                f_l_reg = fl;
                char *f_res = rtl_alloc_f_front();
                rtl_free_f(f_l_reg); rtl_free_f(f_r_reg);
                fprintf(out, "\t%s $%s, $%s, $%s\t# Result: $%s, Opd1: $%s, Opd2:$%s\n",
                        op, f_res, f_l_reg, f_r_reg, f_res, f_l_reg, f_r_reg);
                return f_res;
            } else if (!llf && rlf) {
                f_l_reg = asm_gen_expr(node->child1, out);
                char *fr = rtl_alloc_f_right_leaf();
                asm_load_into(node->child2, fr, out);
                f_r_reg = fr;
                char *f_res = rtl_alloc_f_front();
                rtl_free_f(f_l_reg); rtl_free_f(f_r_reg);
                fprintf(out, "\t%s $%s, $%s, $%s\t# Result: $%s, Opd1: $%s, Opd2:$%s\n",
                        op, f_res, f_l_reg, f_r_reg, f_res, f_l_reg, f_r_reg);
                return f_res;
            } else {
                f_l_reg = asm_gen_expr(node->child1, out);
                f_r_reg = asm_gen_expr(node->child2, out);
                char *f_res = rtl_alloc_f_front();
                rtl_free_f(f_l_reg); rtl_free_f(f_r_reg);
                fprintf(out, "\t%s $%s, $%s, $%s\t# Result: $%s, Opd1: $%s, Opd2:$%s\n",
                        op, f_res, f_l_reg, f_r_reg, f_res, f_l_reg, f_r_reg);
                return f_res;
            }
        }

        /* ── int binary — mirrors rtl_gen_expr exactly ── */
        const char *op = asm_mips_op(lbl);
        const char *regL, *regR;

        if (llf && rlf) {
            if (!rtl_v0_live) {
                char *t_res = rtl_alloc_t_min();
                char *v = rtl_get_v0();
                char *t_r = rtl_alloc_t_min();
                asm_load_into(node->child1, v, out);
                asm_load_into(node->child2, t_r, out);
                rtl_free_reg(v); rtl_free_reg(t_r);
                asm_emit_binop(op, t_res, v, t_r, out);
                return t_res;
            } else {
                char *t_l = rtl_alloc_t_min();
                char *t_res = rtl_alloc_t_min();
                char *t_r = rtl_alloc_t_min();
                asm_load_into(node->child1, t_l, out);
                asm_load_into(node->child2, t_r, out);
                rtl_free_reg(t_l); rtl_free_reg(t_r);
                asm_emit_binop(op, t_res, t_l, t_r, out);
                return t_res;
            }
        }
        else if (llf && !rlf) {
            regR = asm_gen_expr(node->child2, out);
            char *lreg = rtl_alloc_v0_or_t();
            asm_load_into(node->child1, lreg, out);
            regL = lreg;
            char *t_res = rtl_alloc_t_min();
            rtl_free_reg(regL); rtl_free_reg(regR);
            asm_emit_binop(op, t_res, regL, regR, out);
            return t_res;
        }
        else if (!llf && rlf) {
            regL = asm_gen_expr(node->child1, out);
            if (regL[0] == 't') {
                if (!rtl_v0_live) {
                    char *t_r = rtl_alloc_t_min();
                    asm_load_into(node->child2, t_r, out);
                    regR = t_r;
                    char *v = rtl_get_v0();
                    rtl_free_reg(regL); rtl_free_reg(regR);
                    asm_emit_binop(op, v, regL, regR, out);
                    return v;
                } else {
                    char *t_r = rtl_alloc_t_max();
                    asm_load_into(node->child2, t_r, out);
                    regR = t_r;
                    char *t_res = rtl_alloc_t_min();
                    rtl_free_reg(regL); rtl_free_reg(regR);
                    asm_emit_binop(op, t_res, regL, regR, out);
                    return t_res;
                }
            } else {
                char *t_res = rtl_alloc_t_min();
                char *t_r = rtl_alloc_t_min();
                asm_load_into(node->child2, t_r, out);
                regR = t_r;
                rtl_free_reg(regL); rtl_free_reg(regR);
                asm_emit_binop(op, t_res, regL, regR, out);
                return t_res;
            }
        }
        else {
            regL = asm_gen_expr(node->child1, out);
            regR = asm_gen_expr(node->child2, out);
            const char *result;
            if (regL[0] == 't' && regR[0] == 't') {
                if (!rtl_v0_live) {
                    char *v = rtl_get_v0();
                    rtl_free_reg(regL); rtl_free_reg(regR);
                    asm_emit_binop(op, v, regL, regR, out);
                    result = v;
                } else {
                    char *t = rtl_alloc_t_min();
                    rtl_free_reg(regL); rtl_free_reg(regR);
                    asm_emit_binop(op, t, regL, regR, out);
                    result = t;
                }
            } else {
                char *t = rtl_alloc_t_min();
                rtl_free_reg(regL); rtl_free_reg(regR);
                asm_emit_binop(op, t, regL, regR, out);
                result = t;
            }
            return result;
        }
    }

    /* ── ternary ?: ── */
    if (strcmp(lbl, "Condition: ?:") == 0) {
    const char *cond = asm_gen_expr(node->child1, out);
    int stemp_idx = asm_stemp_cnt++;
    int stemp_off = asm_ternary_offsets[stemp_idx];
    char *Lfalse = asm_new_label(); char *Lend = asm_new_label();
    int tf = (strcmp(node_type(node->child2), "float") == 0);

    char *not_r;
    if (cond[0] == 't' && !rtl_v0_live) not_r = rtl_get_v0();
    else not_r = rtl_alloc_t();
    rtl_free_reg(cond);
    fprintf(out, "\txori $%s, $%s, 1\t# Result: $%s, Opd1: $%s\n", not_r, cond, not_r, cond);
    fprintf(out, "\tbgtz $%s, %s\n", not_r, Lfalse);
    rtl_free_reg(not_r);

    rtl_reset_regs();
    const char *tr = asm_gen_expr(node->child2, out);
    if (tf) fprintf(out, "\ts.d $%s, %d($fp)\t\t\t# Dest: $%s\n", tr, stemp_off, tr);
    else    fprintf(out, "\tsw $%s, %d($fp)\t\t\t# Dest: $%s\n",  tr, stemp_off, tr);
    rtl_free_reg(tr);
    fprintf(out, "\tj %s\n", Lend);

    fprintf(out, "%s:\n", Lfalse); free(Lfalse);
    rtl_reset_regs();
    const char *fr = asm_gen_expr(node->child3, out);
    if (tf) fprintf(out, "\ts.d $%s, %d($fp)\t\t\t# Dest: $%s\n", fr, stemp_off, fr);
    else    fprintf(out, "\tsw $%s, %d($fp)\t\t\t# Dest: $%s\n",  fr, stemp_off, fr);
    rtl_free_reg(fr);

    fprintf(out, "%s:\n", Lend); free(Lend);
    rtl_reset_regs();
    if (tf) {
        char *f = rtl_alloc_f_front();
        fprintf(out, "\tl.d $%s, %d($fp)\t\t\t# Source: stemp%d\n", f, stemp_off, stemp_idx);
        return f;
    } else {
        char *v = rtl_get_v0();
        fprintf(out, "\tlw $%s, %d($fp)\t\t\t# Source: stemp%d\n", v, stemp_off, stemp_idx);
        return v;
    }
}

    return rtl_get_v0();
}

/* ════════════════════════════════════════════
   WRITE HELPER
   ════════════════════════════════════════════ */
static void asm_gen_write(const char *val_lbl, ASTNode *expr, FILE *out) {
    const char *lbl = expr ? expr->label : val_lbl;
    if (strncmp(lbl, "Str:", 4) == 0) {
        char buf[256]; rtl_str(lbl, buf); int idx = rtl_add_string(buf);
        char sn[32]; sprintf(sn, "_str_%d", idx);
        fprintf(out, "\tli $v0, 4\t\t\t# Loading 4 in v0 to indicate syscall to print string value\n");
        fprintf(out, "\tla $a0, %s\t\t\t# String = %s\n", sn, buf);
        fprintf(out, "\tsyscall\n");
    } else if (strncmp(lbl, "Name:", 5) == 0) {
        char buf[128]; rtl_name(lbl, buf);
        if (strstr(lbl, "<float>")) {
            fprintf(out, "\tli $v0, 3\t\t\t# Loading 3 in v0 to indicate syscall to print double value\n");
            asm_load_float_var(buf, "f12", out);
            fprintf(out, "\tsyscall\n");
        } else if (strstr(lbl, "<string>")) {
            fprintf(out, "\tli $v0, 4\t\t\t# Loading 4 in v0 to indicate syscall to print string value\n");
            asm_load_var(buf, "a0", out); fprintf(out, "\tsyscall\n");
        } else {
            fprintf(out, "\tli $v0, 1\t\t\t# Loading 1 in v0 to indicate syscall to print integer value\n");
            asm_load_var(buf, "a0", out); fprintf(out, "\tsyscall\n");
        }
    } else if (strncmp(lbl, "Num:", 4) == 0) {
        char buf[128]; rtl_num(lbl, buf);
        if (strstr(lbl, "<float>")) {
            double v = atof(buf);
            fprintf(out, "\tli $v0, 3\t\t\t# Loading 3 in v0 to indicate syscall to print double value\n");
            fprintf(out, "\tli.d $f12, %.2f\t\t\t# Loading float number %.6f\n", v, v);
            fprintf(out, "\tsyscall\n");
        } else {
            fprintf(out, "\tli $v0, 1\t\t\t# Loading 1 in v0 to indicate syscall to print integer value\n");
            fprintf(out, "\tli $a0, %s\t\t\t# Moving the value to be printed into register a0\n", buf);
            fprintf(out, "\tsyscall\n");
        }
    } else if (expr && !is_leaf(expr)) {
        const char *r = asm_gen_expr(expr, out);
        fprintf(out, "\tli $v0, 1\t\t\t# Loading 1 in v0 to indicate syscall to print integer value\n");
        fprintf(out, "\tmove $a0, $%s\t\t\t# Moving the value to be printed into register a0\n", r);
        fprintf(out, "\tsyscall\n");
    }
}

/* ════════════════════════════════════════════
   STATEMENT GENERATOR
   ════════════════════════════════════════════ */
static void asm_gen_stmt(ASTNode *node, FILE *out) {
    while (node) {
        const char *lbl = node->label;

        rtl_reset_regs();   /* reset register allocator per statement, like RTL */

        /* assignment */
        if (strcmp(lbl, "Asgn:") == 0) {
            char dest[128]; rtl_name(node->child1->label, dest);
            int df = (strstr(node->child1->label, "<float>") != NULL);
            ASTNode *rhs = node->child2;

            if (is_leaf(rhs) && strncmp(rhs->label, "Str:", 4) == 0) {
                char buf[256]; rtl_str(rhs->label, buf); int idx = rtl_add_string(buf);
                char sn[32]; sprintf(sn, "_str_%d", idx);
                char *v = rtl_get_v0();
                fprintf(out, "\tla $%s, %s\t\t\t# String = %s\n", v, sn, buf);
                asm_store_var(dest, v, out);
            } else if (is_leaf(rhs) && strncmp(rhs->label, "FuncCall:", 9) != 0) {
                if (df) {
                    char *f = rtl_alloc_f_front();
                    asm_load_into(rhs, f, out);
                    asm_store_float_var(dest, f, out);
                } else {
                    char *v = rtl_get_v0();
                    asm_load_into(rhs, v, out);
                    asm_store_var(dest, v, out);
                }
            } else {
                const char *r = asm_gen_expr(rhs, out);
                int rfc = (strncmp(rhs->label, "FuncCall:", 9) == 0);
                int rff = rfc && (strcmp(node_type(rhs), "float") == 0);
                if (rfc && rff) {
                    char *f = rtl_alloc_f_front();
                    fprintf(out, "\tmov.d $%s, $f0\t\t\t# store function call return in $f0 so that another function call can be made\n", f);
                    asm_store_float_var(dest, f, out);
                } else if (rfc) {
                    char *v = rtl_get_v0();
                    fprintf(out, "\tmove $%s, $v1\t\t\t# store function call return in $v1 so that another function call can be made\n", v);
                    asm_store_var(dest, v, out);
                } else if (df) {
                    asm_store_float_var(dest, r, out);
                } else {
                    asm_store_var(dest, r, out);
                }
            }
        }

        /* read */
        else if (strncmp(lbl, "Read:", 5) == 0) {
            const char *p = strstr(lbl, ": Name : "); if (p) p += 9; else p = lbl + 6;
            char tmp[64]; strcpy(tmp, p); char *cut = strchr(tmp, '<'); if (cut) *cut = '\0';
            if (strstr(lbl, "<float>")) {
                fprintf(out, "\tli $v0, 7\t\t\t# Loading 7 in v0 to indicate syscall to read double value\n");
                fprintf(out, "\tsyscall\n"); asm_store_float_var(tmp, "f0", out);
            } else {
                fprintf(out, "\tli $v0, 5\t\t\t# Loading 5 in v0 to indicate syscall to read integer value\n");
                fprintf(out, "\tsyscall\n"); asm_store_var(tmp, "v0", out);
            }
        }

        /* write */
        else if (strncmp(lbl, "Write:", 6) == 0) {
            if (node->child1) asm_gen_write(NULL, node->child1, out);
            else asm_gen_write(lbl + 7, NULL, out);
        }

        /* if / if-else */
        else if (strcmp(lbl, "If:") == 0) {
    /* Generate condition directly to out first */
    const char *cr = asm_gen_expr(node->child1, out);

    /* Buffer then-body so inner labels get lower numbers */
    char *tbuf = NULL; size_t tsz = 0;
    FILE *tmem = open_memstream(&tbuf, &tsz);
    if (node->child2) asm_gen_stmt(node->child2, tmem);
    fclose(tmem);

    rtl_reset_regs();
if (cr[0] == 'v') rtl_v0_live = true;

    /* Allocate if labels AFTER body buffering */
    char *Le  = asm_new_label();
    char *Lel = node->child3 ? asm_new_label() : strdup(Le);

    char *not_r;
    if (cr[0] == 't' && !rtl_v0_live) not_r = rtl_get_v0();
    else not_r = rtl_alloc_t();
    rtl_free_reg(cr);
    fprintf(out, "\txori $%s, $%s, 1\t# Result: $%s, Opd1: $%s\n", not_r, cr, not_r, cr);
    fprintf(out, "\tbgtz $%s, %s\n", not_r, Lel);
    rtl_free_reg(not_r);

    fprintf(out, "%s", tbuf); free(tbuf);

    if (node->child3) {
    fprintf(out, "\tj %s\n", Le);
    fprintf(out, "%s:\n", Lel);
    asm_gen_stmt(node->child3, out);
} else {
    fprintf(out, "\tj %s\n", Le);
}
fprintf(out, "%s:\n", Le); free(Lel); free(Le);
}

        /* while */
        else if (strcmp(lbl, "While:") == 0) {
    /* Buffer body FIRST so its inner labels get lower numbers */
    char *bbuf = NULL; size_t bsz = 0;
    FILE *bmem = open_memstream(&bbuf, &bsz);
    if (node->child2) asm_gen_stmt(node->child2, bmem);
    fclose(bmem);

    /* Allocate while labels AFTER body — they get higher numbers */
    char *Ls = asm_new_label();
    char *Le = asm_new_label();

    /* Generate condition fresh */
    rtl_reset_regs();
    fprintf(out, "%s:\n", Ls);
    const char *cr = asm_gen_expr(node->child1, out);
    bool cr_is_t = (cr && cr[0] == 't');
    bool v0_was_live = rtl_v0_live;

    char *not_r;
    if (cr_is_t && !v0_was_live) not_r = rtl_get_v0();
    else not_r = rtl_alloc_t();
    rtl_free_reg(cr);
    fprintf(out, "\txori $%s, $%s, 1\t# Result: $%s, Opd1: $%s\n", not_r, cr, not_r, cr);
    fprintf(out, "\tbgtz $%s, %s\n", not_r, Le);
    rtl_free_reg(not_r);

    /* Emit buffered body */
    fprintf(out, "%s", bbuf); free(bbuf);

    fprintf(out, "\tj %s\n", Ls);
    fprintf(out, "%s:\n", Le);
    free(Ls); free(Le);
}

        /* do-while */
        else if (strcmp(lbl, "Do:") == 0) {
    /* Buffer body first so inner labels get lower numbers */
    char *bbuf = NULL; size_t bsz = 0;
    FILE *bmem = open_memstream(&bbuf, &bsz);
    if (node->child1) asm_gen_stmt(node->child1, bmem);
    fclose(bmem);

    /* Allocate do-while label AFTER body buffering */
    char *Ls = asm_new_label();

    fprintf(out, "%s:\n", Ls);
    fprintf(out, "%s", bbuf); free(bbuf);

    rtl_reset_regs();
    const char *cr = asm_gen_expr(node->child2, out);
    fprintf(out, "\tbgtz $%s, %s\n", cr, Ls);
    free(Ls);
}

        /* return */
        else if (strncmp(lbl, "Return:", 7) == 0) {
            int rf = (strcmp(asm_current_ret_type, "float") == 0);
            if (node->child1) {
                const char *r = asm_gen_expr(node->child1, out);
                if (rf) fprintf(out, "\ts.d $%s, %d($fp)\t\t\t# Dest: stemp0\n", r, asm_stemp_offset);
                else fprintf(out, "\tsw $%s, %d($fp)\t\t\t# Dest: stemp0\n", r, asm_stemp_offset);
                fprintf(out, "\tj %s\n", asm_ret_label);
                asm_has_return = 1;
            } else {
                fprintf(out, "\tj epilogue_%s\n", asm_current_func);
            }
        }

        /* function call as statement */
        else if (strncmp(lbl, "FuncCall:", 9) == 0) {
            asm_gen_expr(node, out);
        }

        node = node->next;
    }
}

static void asm_prescan_strings_expr(ASTNode *node) {
    if (!node) return;
    if (strncmp(node->label, "Str:", 4) == 0) {
        char buf[256]; rtl_str(node->label, buf); rtl_add_string(buf);
        return;
    }
    if (strncmp(node->label, "FuncCall:", 9) == 0) {
        ASTNode *arg = node->child1;
        while (arg) {
            ASTNode *ns = arg->next; arg->next = NULL;
            asm_prescan_strings_expr(arg);
            arg->next = ns; arg = ns;
        }
        return;
    }
    asm_prescan_strings_expr(node->child1);
    asm_prescan_strings_expr(node->child2);
    asm_prescan_strings_expr(node->child3);
}

static void asm_prescan_strings_stmt(ASTNode *node) {
    while (node) {
        const char *lbl = node->label;
        if (strncmp(lbl, "Write:", 6) == 0 && strstr(lbl, "Str:")) {
            /* string embedded in label — register it first */
            const char *sp = strstr(lbl, "Str:") + 5;
            const char *cut = strrchr(sp, '<');
            char buf[256];
            if (cut) { int len=cut-sp; strncpy(buf,sp,len); buf[len]='\0'; }
            else strcpy(buf, sp);
            rtl_add_string(buf);
        } else if (strncmp(lbl, "Write:", 6) == 0 && node->child1) {
            asm_prescan_strings_expr(node->child1);
        } else if (strcmp(lbl, "Asgn:") == 0) {
            asm_prescan_strings_expr(node->child2);
        } else if (strncmp(lbl, "Return:", 7) == 0 && node->child1) {
            asm_prescan_strings_expr(node->child1);
        } else if (strcmp(lbl, "If:") == 0) {
            asm_prescan_strings_expr(node->child1);
            asm_prescan_strings_stmt(node->child2);
            asm_prescan_strings_stmt(node->child3);
        } else if (strcmp(lbl, "While:") == 0) {
            asm_prescan_strings_expr(node->child1);
            asm_prescan_strings_stmt(node->child2);
        } else if (strcmp(lbl, "Do:") == 0) {
            asm_prescan_strings_stmt(node->child1);
            asm_prescan_strings_expr(node->child2);
        } else if (strncmp(lbl, "FuncCall:", 9) == 0) {
            ASTNode *arg = node->child1;
            while (arg) {
                ASTNode *ns = arg->next; arg->next = NULL;
                asm_prescan_strings_expr(arg);
                arg->next = ns; arg = ns;
            }
        }
        node = node->next;
    }
}

/* ════════════════════════════════════════════
   ENTRY POINT
   ════════════════════════════════════════════ */
inline void print_asm(FILE *out) {
    int indices[MAX_FUNCS]; int count = 0;
    for (int i = 0; i < func_count; i++)
        if (func_table[i].defined) indices[count++] = i;

    int gen_order[MAX_FUNCS];
    for (int i = 0; i < count; i++) gen_order[i] = indices[i];
    for (int i = 0; i < count-1; i++)
        for (int j = i+1; j < count; j++)
            if (strcmp(func_table[gen_order[i]].name, func_table[gen_order[j]].name) > 0)
                { int t = gen_order[i]; gen_order[i] = gen_order[j]; gen_order[j] = t; }

    rtl_str_count = 0; rtl_str_table.clear(); asm_label_count = 0; asm_stemp_cnt = 0;

    /* pre-register strings in insertion order */
    for (int i = 0; i < func_count; i++) {
        if (!func_table[i].defined || !func_table[i].body) continue;
        ASTNode *n = func_table[i].body;
        asm_prescan_strings_stmt(func_table[i].body);
    }

    /* pre-allocate return labels in insertion order */
    int ret_label_nums[MAX_FUNCS];
    for (int i = 0; i < MAX_FUNCS; i++) ret_label_nums[i] = -1;
    for (int i = 0; i < func_count; i++)
        if (func_table[i].defined && strcmp(func_table[i].ret_type, "void") != 0)
            ret_label_nums[i] = asm_label_count++;

    /* .data section */
    if (sym_count > 0 || !rtl_str_table.empty()) {
        fprintf(out, "\n    .data\n");
        for (int i = 0; i < sym_count; i++) {
            if (strcmp(sym_table[i].type, "float") == 0)
                fprintf(out, "%s_:\t.double 0.0\n", sym_table[i].name);
            else if (strcmp(sym_table[i].type, "int") == 0 || strcmp(sym_table[i].type, "bool") == 0)
                fprintf(out, "%s_:\t.word 0\n", sym_table[i].name);
            else if (strcmp(sym_table[i].type, "string") == 0)
                fprintf(out, "%s_:\t.word 0\n", sym_table[i].name);
        }
        for (int i = 0; i < (int)rtl_str_table.size(); i++)
            fprintf(out, "_str_%d:\t.asciiz %s\n", i, rtl_str_table[i].c_str());
    }

    /* generate ASM in raw-name order, buffer each */
    char *bufs[MAX_FUNCS] = {NULL};
    for (int k = 0; k < count; k++) {
        int idx = gen_order[k]; FuncSymbol *fn = &func_table[idx];
        memcpy(local_table, fn->local_syms, fn->local_sym_count * sizeof(Symbol));
        local_count = fn->local_sym_count;
        asm_collect_locals_for_func(fn);

        int nv = (strcmp(fn->ret_type, "void") != 0);
        asm_has_return = 0; asm_current_ret_type = fn->ret_type;

        char fl[128];
        if (strcmp(fn->name, "main") == 0) strcpy(fl, "main");
        else sprintf(fl, "%s_", fn->name);
        strcpy(asm_current_func, fl);

        if (nv) sprintf(asm_ret_label, "Label%d", ret_label_nums[idx]);

        char *buf = NULL; size_t sz = 0; FILE *mem = open_memstream(&buf, &sz);
        fprintf(mem, "    .text\t\t\t# The .text assembler directive indicates\n");
        fprintf(mem, "    .globl %s\t\t\t# The following is the code (as opposed to data)\n", fl);
        fprintf(mem, "%s:\t\t\t\t# .globl makes main know to the outside of the program.\n", fl);
        fprintf(mem, "# Prologue begins\n");
        fprintf(mem, "    sw $ra, 0($sp)\t\t\t# Save the return address\n");
        fprintf(mem, "    sw $fp, -4($sp)\t\t\t# Save the frame pointer\n");
        fprintf(mem, "    sub $fp, $sp, 4\t\t\t# Update the frame pointer\n");
        fprintf(mem, "    sub $sp, $sp, %d\t\t\t# Make space for the locals\n", asm_frame_size);
        fprintf(mem, "# Prologue ends\n\n");

        asm_has_return = 0;
        asm_current_ret_type = fn->ret_type;
        asm_stemp_cnt = 0;

        if (fn->body) asm_gen_stmt(fn->body, mem);

        if (nv && asm_has_return) {
            int rf = (strcmp(fn->ret_type, "float") == 0);
            fprintf(mem, "%s:\n", asm_ret_label);
            if (rf) fprintf(mem, "    l.d $f0, %d($fp)\t\t\t# Moving the value to be printed into register a0\n", asm_stemp_offset);
            else fprintf(mem, "    lw $v1, %d($fp)\t\t\t# Moving the value to be printed into register a0\n", asm_stemp_offset);
            fprintf(mem, "    j epilogue_%s\n", fl);
        }

        fprintf(mem, "epilogue_%s:\n", fl);
        fprintf(mem, "    add $sp, $sp, %d\t\t\t# Increment stack pointer for local variables\n", asm_frame_size);
        fprintf(mem, "    lw $fp, -4($sp)\t\t\t# Set $fp to $sp-4\n");
        fprintf(mem, "    lw $ra, 0($sp)\t\t\t# Save ra\n");
        fprintf(mem, "    jr $ra\t\t\t\t# Jump back to the called procedure\n");
        fprintf(mem, "# Epilogue Ends\n");
        fclose(mem); bufs[idx] = buf;
    }

    /* print in name_ order */
    int po[MAX_FUNCS];
    for (int i = 0; i < count; i++) po[i] = indices[i];
    for (int i = 0; i < count-1; i++)
        for (int j = i+1; j < count; j++) {
            char a[128], b[128];
            sprintf(a, "%s_", func_table[po[i]].name);
            sprintf(b, "%s_", func_table[po[j]].name);
            if (strcmp(a, b) > 0) { int t = po[i]; po[i] = po[j]; po[j] = t; }
        }
    for (int k = 0; k < count; k++) {
        int idx = po[k];
        if (bufs[idx]) { fprintf(out, "%s", bufs[idx]); if (k < count-1) fprintf(out, "\n"); free(bufs[idx]); }
    }
}

#endif