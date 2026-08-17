#ifndef RTL_HPP
#define RTL_HPP

#include "ast.hpp"
#include <string>
#include <vector>
#include <set>

/* ════════════════════════════════════════════
   STRING TABLE & LABEL COUNTER
   ════════════════════════════════════════════ */
static std::vector<std::string> rtl_str_table;
static int rtl_str_count   = 0;
static int rtl_label_count = 0;
static int rtl_stemp_cnt   = 0;

static char rtl_ret_stemp[16];
static char rtl_ret_label[16];
static int  rtl_has_return = 0;
static const char *rtl_current_func_ret_type = "void";

/* ── register allocators ── */
static int  rtl_t_max   = 0;
static int  rtl_a_max   = 0;
static int  rtl_f_next  = 1;
static bool rtl_v0_live = false;
static bool rtl_float_cmp_comment_printed = false;

static std::set<int> rtl_t_free;
static std::vector<int> rtl_a_free;
static std::set<int> rtl_f_free;

/* ── t allocators ── */
static char *rtl_alloc_t_min()
{
    char *b = (char*)malloc(8);
    int n;
    if (!rtl_t_free.empty()) {
        n = *rtl_t_free.begin();
        rtl_t_free.erase(rtl_t_free.begin());
    } else { n = rtl_t_max++; }
    sprintf(b, "t%d", n);
    return b;
}

static char *rtl_alloc_t_max()
{
    char *b = (char*)malloc(8);
    int n;
    if (!rtl_t_free.empty()) {
        auto it = std::prev(rtl_t_free.end());
        n = *it; rtl_t_free.erase(it);
    } else { n = rtl_t_max++; }
    sprintf(b, "t%d", n);
    return b;
}

static char *rtl_alloc_t() { return rtl_alloc_t_min(); }

/* ── v0 allocator ── */
static char *rtl_get_v0()
{
    rtl_v0_live = true;
    char *b = (char*)malloc(4);
    strcpy(b, "v0");
    return b;
}

static char *rtl_alloc_v0_or_t()
{
    if (!rtl_v0_live) return rtl_get_v0();
    return rtl_alloc_t_min();
}

/* ── a allocator ── */
static char *rtl_alloc_a()
{
    char *b = (char*)malloc(8);
    int n;
    if (!rtl_a_free.empty()) { n = rtl_a_free.back(); rtl_a_free.pop_back(); }
    else { n = rtl_a_max++; }
    sprintf(b, "a%d", n);
    return b;
}

/* ── f allocators ── */
static char *rtl_alloc_f_front()
{
    char *b = (char*)malloc(8);
    int n;
    if (!rtl_f_free.empty()) {
        n = *rtl_f_free.begin();
        rtl_f_free.erase(rtl_f_free.begin());
    } else { n = rtl_f_next++; }
    sprintf(b, "f%d", n * 2);
    return b;
}

static char *rtl_alloc_f_right_leaf()
{
    char *b = (char*)malloc(8);
    int n;
    if (rtl_f_free.size() > 1) {
        auto it = std::prev(rtl_f_free.end());
        n = *it; rtl_f_free.erase(it);
    } else { n = rtl_f_next++; }
    sprintf(b, "f%d", n * 2);
    return b;
}

static void rtl_free_f(const char *reg)
{
    if (!reg || reg[0] != 'f') return;
    int num = atoi(reg + 1);
    rtl_f_free.insert(num / 2);
}

static void rtl_free_reg(const char *reg)
{
    if (!reg) return;
    if      (reg[0] == 'v') { rtl_v0_live = false; }
    else if (reg[0] == 't') { rtl_t_free.insert(atoi(reg+1)); }
    else if (reg[0] == 'a') { rtl_a_free.push_back(atoi(reg+1)); }
    else if (reg[0] == 'f') { rtl_free_f(reg); }
}

static void rtl_reset_regs()
{
    rtl_t_max = 0; rtl_t_free.clear();
    rtl_a_max = 0; rtl_a_free.clear();
    rtl_f_next = 1; rtl_f_free.clear();
    rtl_v0_live = false;
    rtl_float_cmp_comment_printed = false;
}

static int rtl_add_string(const char *s)
{
    for (int i = 0; i < (int)rtl_str_table.size(); i++)
        if (rtl_str_table[i] == std::string(s))
            return i;
    rtl_str_table.push_back(std::string(s));
    return rtl_str_count++;
}

static char *rtl_new_label(void)
{
    char *b = (char*)malloc(16);
    sprintf(b, "Label%d", rtl_label_count++);
    return b;
}

static void rtl_name(const char *lbl, char *buf)
{
    const char *p = lbl + 6;
    const char *cut = strchr(p, '<');
    if (cut) { int n=cut-p; strncpy(buf,p,n); buf[n]='\0'; }
    else strcpy(buf, p);
}

static void rtl_num(const char *lbl, char *buf)
{
    const char *p = lbl + 5;
    const char *cut = strchr(p, '<');
    if (cut) { int n=cut-p; strncpy(buf,p,n); buf[n]='\0'; }
    else strcpy(buf, p);
}

static void rtl_str(const char *lbl, char *buf)
{
    const char *p = lbl + 5;
    const char *cut = strrchr(p, '<');
    if (cut) { int n=cut-p; strncpy(buf,p,n); buf[n]='\0'; }
    else strcpy(buf, p);
}

static void rtl_load_into(ASTNode *node, const char *reg, FILE *out)
{
    const char *lbl = node->label;
    char buf[128];
    if (strncmp(lbl, "Name:", 5) == 0) {
        rtl_name(lbl, buf);
        int is_float = (strstr(lbl, "<float>") != NULL);
        if (is_float) {
            fprintf(out, "    %-12s%s <- %-10s;; Loading variable %s into register\n",
                    "load.d:", reg, buf, buf);
        } else {
            fprintf(out, "    %-12s%s <- %-10s;; Loading variable %s into register\n",
                    "load:", reg, buf, buf);
        }
    } else if (strncmp(lbl, "Num:", 4) == 0) {
        rtl_num(lbl, buf);
        int is_float = (strstr(lbl, "<float>") != NULL);
        if (is_float) {
            double val = atof(buf);
            fprintf(out, "    %-12s%s <- %-10s;; Loading float number %f\n",
                    "iLoad.d:", reg, buf, val);
        } else {
            fprintf(out, "    %-12s%s <- %-10s;; Loading integer number %s\n",
                    "iLoad:", reg, buf, buf);
        }
    } else if (strncmp(lbl, "Str:", 4) == 0) {
        char buf2[256]; rtl_str(lbl, buf2);
        int idx = rtl_add_string(buf2);
        char sname[32]; sprintf(sname, "_str_%d", idx);
        fprintf(out, "    %-12s%s <- %-10s;; String = %s\n",
                "load_addr:", reg, sname, buf2);
    } else if (strncmp(lbl, "Bool:", 5) == 0) {
        const char *val = strstr(lbl, "true") ? "1" : "0";
        fprintf(out, "    %-12s%s <- %s\n", "iLoad:", reg, val);
    }
}

static const char *rtl_binop(const char *lbl)
{
    if (strstr(lbl, "Plus"))   return "add";
    if (strstr(lbl, "Minus"))  return "sub";
    if (strstr(lbl, "Mult"))   return "mul";
    if (strstr(lbl, "Div"))    return "div";
    if (strstr(lbl, ": GT"))   return "sgt";
    if (strstr(lbl, ": LT"))   return "slt";
    if (strstr(lbl, ": GE"))   return "sge";
    if (strstr(lbl, ": LE"))   return "sle";
    if (strstr(lbl, ": EQ"))   return "seq";
    if (strstr(lbl, ": NE"))   return "sne";
    if (strstr(lbl, ": AND"))  return "and";
    if (strstr(lbl, ": OR"))   return "or";
    return "?";
}

static const char *rtl_binop_float(const char *lbl)
{
    if (strstr(lbl, "Plus"))   return "add.d";
    if (strstr(lbl, "Minus"))  return "sub.d";
    if (strstr(lbl, "Mult"))   return "mul.d";
    if (strstr(lbl, "Div"))    return "div.d";
    if (strstr(lbl, ": GT"))   return "sgt.d";
    if (strstr(lbl, ": LT"))   return "slt.d";
    if (strstr(lbl, ": GE"))   return "sge.d";
    if (strstr(lbl, ": LE"))   return "sle.d";
    if (strstr(lbl, ": EQ"))   return "seq.d";
    if (strstr(lbl, ": NE"))   return "sne.d";
    return "?";
}

static void rtl_gen_stmt(ASTNode *node, FILE *out);

/* ════════════════════════════════════════════
   EXPRESSION GENERATOR
   ════════════════════════════════════════════ */
static const char *rtl_gen_expr(ASTNode *node, FILE *out)
{
    if (!node) return "v0";
    const char *lbl = node->label;
    int is_float = (strcmp(node_type(node), "float") == 0);

    /* ── function call ── */
    if (strncmp(lbl, "FuncCall:", 9) == 0) {
        char fname[64];
        const char *p = lbl + 10;
        const char *cut = strchr(p, '<');
        if (cut) { int n=cut-p; strncpy(fname,p,n); fname[n]='\0'; }
        else strcpy(fname, p);

        int is_float_ret = (strcmp(node_type(node), "float") == 0);

        /* push args in REVERSE order */
        int argc = 0;
        ASTNode *arg = node->child1;
        while (arg) { argc++; arg = arg->next; }

        if (argc > 0) {
            ASTNode *args[MAX_PARAMS];
            arg = node->child1;
            for (int i = 0; i < argc; i++) { args[i] = arg; arg = arg->next; }

            /* STEP 1: pre-generate complex (non-leaf) args in forward order */
            const char *pre_regs[MAX_PARAMS] = {NULL};
            for (int i = 0; i < argc; i++) {
                ASTNode *next_save = args[i]->next;
                args[i]->next = NULL;
                if (!is_leaf(args[i])) {
                    pre_regs[i] = rtl_gen_expr(args[i], out);
                }
                args[i]->next = next_save;
            }

            /* STEP 2: push in reverse — leaf args generated here, complex just pushed */
            for (int i = argc - 1; i >= 0; i--) {
                ASTNode *next_save = args[i]->next;
                args[i]->next = NULL;
                if (is_leaf(args[i])) {
                    int arg_is_float = (strcmp(node_type(args[i]), "float") == 0);
                    if (arg_is_float) {
                        char *f = rtl_alloc_f_front();
                        rtl_load_into(args[i], f, out);
                        fprintf(out, "    %-12s%-20s;; Push parameter onto stack\n", "push:", f);
                        rtl_free_f(f);
                    } else {
                        char *v = rtl_get_v0();
                        rtl_load_into(args[i], v, out);
                        fprintf(out, "    %-12s%-20s;; Push parameter onto stack\n", "push:", v);
                        rtl_free_reg(v);
                    }
                } else {
                    fprintf(out, "    %-12s%-20s;; Push parameter onto stack\n", "push:", pre_regs[i]);
                }
                args[i]->next = next_save;
                rtl_reset_regs();
            }
        }

        /* emit call */
        if (is_float_ret)
            fprintf(out, "    f0 = call %s_\n", fname);
        else
            fprintf(out, "    v1 = call %s_\n", fname);

        /* pop args in forward order */
        if (argc > 0) {
            ASTNode *args2[MAX_PARAMS];
            arg = node->child1;
            for (int i = 0; i < argc; i++) { args2[i] = arg; arg = arg->next; }
            for (int i = 0; i < argc; i++) {
                char pname[64];
                const char *plbl = args2[i]->label;
                if (strncmp(plbl, "Name:", 5) == 0) {
                    rtl_name(plbl, pname);
                } else {
                    sprintf(pname, "arg%d", i);
                }
                fprintf(out, "    %-12s%-20s;; Pop parameter %s from stack\n", "pop", "", pname);
            }
        }

        if (is_float_ret)
            return strdup("f0");
        else
            return strdup("v1");
    }

    /* ── leaf ── */
    if (!node->child1 && !node->child2 && !node->child3) {
        if (is_float) {
            char *f = rtl_alloc_f_front();
            rtl_load_into(node, f, out);
            return f;
        } else {
            char *reg = rtl_alloc_v0_or_t();
            rtl_load_into(node, reg, out);
            return reg;
        }
    }

    /* ── unary minus ── */
    if (strncmp(lbl, "Arith: Uminus", 13) == 0) {
        const char *r = rtl_gen_expr(node->child1, out);
        if (r[0] == 'f') {
            char *f = rtl_alloc_f_front();
            rtl_free_f(r);
            fprintf(out, "    %-12s%s <- %s\n", "uminus.d:", f, r);
            return f;
        } else {
            char *res;
            if (r[0] == 't' && !rtl_v0_live)
                res = rtl_get_v0();
            else
                res = rtl_alloc_t();
            rtl_free_reg(r);
            fprintf(out, "    %-12s%s <- %s\n", "uminus:", res, r);
            return res;
        }
    }

    /* ── logical NOT ── */
    if (strncmp(lbl, "Condition: NOT", 14) == 0) {
        int child_is_float = node->child1 &&
                             (strcmp(node_type(node->child1), "float") == 0 ||
                              (node->child1->child1 &&
                               strcmp(node_type(node->child1->child1), "float") == 0));
        if (child_is_float) {
    /* generate normally — no movf/movt change */
    const char *r = rtl_gen_expr(node->child1, out);
    char *res;
    if (r[0] == 't' && !rtl_v0_live)
        res = rtl_get_v0();
    else
        res = rtl_alloc_t();
    rtl_free_reg(r);
    fprintf(out, "    %-12s%s <- %s\n", "not:", res, r);
    return res;
}
        const char *r = rtl_gen_expr(node->child1, out);
        char *res;
        if (r[0] == 't' && !rtl_v0_live)
            res = rtl_get_v0();
        else
            res = rtl_alloc_t();
        rtl_free_reg(r);
        fprintf(out, "    %-12s%s <- %s\n", "not:", res, r);
        return res;
    }

    /* ── binary op ── */
    if (node->child1 && node->child2 && !node->child3) {
        bool llf = is_leaf(node->child1);
        bool rlf = is_leaf(node->child2);

        /* ── float comparison (bool result, float operands) ── */
        int child_is_float = node->child1 &&
                             (strcmp(node_type(node->child1), "float") == 0);
        if (!is_float && child_is_float) {
            const char *op_str;
            if      (strstr(lbl, ": GT")) op_str = "sle.d:";
            else if (strstr(lbl, ": GE")) op_str = "slt.d:";
            else if (strstr(lbl, ": LT")) op_str = "slt.d:";
            else if (strstr(lbl, ": LE")) op_str = "sle.d:";
            else if (strstr(lbl, ": EQ")) op_str = "seq.d:";
            else if (strstr(lbl, ": NE")) op_str = "seq.d:";
            else op_str = "scmp.d:";

            const char *f_l_reg = nullptr;
            const char *f_r_reg = nullptr;

            if (llf && rlf) {
                char *f_l = rtl_alloc_f_front();
                char *f_r = rtl_alloc_f_front();
                rtl_load_into(node->child1, f_l, out);
                rtl_load_into(node->child2, f_r, out);
                f_l_reg = f_l; f_r_reg = f_r;
            }
            else if (llf && !rlf) {
                f_r_reg = rtl_gen_expr(node->child2, out);
                char *f_l = rtl_alloc_f_front();
                rtl_load_into(node->child1, f_l, out);
                f_l_reg = f_l;
            }
            else if (!llf && rlf) {
                f_l_reg = rtl_gen_expr(node->child1, out);
                char *f_r = rtl_alloc_f_front();
                rtl_load_into(node->child2, f_r, out);
                f_r_reg = f_r;
            }
            else {
                f_l_reg = rtl_gen_expr(node->child1, out);
                f_r_reg = rtl_gen_expr(node->child2, out);
            }

            if (!rtl_float_cmp_comment_printed) {
                fprintf(out, "    %-12s%s , %-11s;; Negating the condition and using le for gt, lt for ge, and eq for neq, for float values\n",
                        op_str, f_l_reg, f_r_reg);
                fprintf(out, "                                ;; because gt, ge, and neq operations are not supported by the coprocessor\n");
                rtl_float_cmp_comment_printed = true;
            } else {
                fprintf(out, "    %-12s%s , %s\n", op_str, f_l_reg, f_r_reg);
            }

            rtl_free_f(f_l_reg);
            rtl_free_f(f_r_reg);

            char *v = rtl_get_v0();
            char *t = rtl_alloc_t();
            fprintf(out, "    %-12s%s <- %-10s\n", "iLoad:", v, "1");
            fprintf(out, "    %-12s%s <- zero\n", "move:", t);
            bool is_direct = (strstr(lbl, ": LT") || strstr(lbl, ": LE") || strstr(lbl, ": EQ"));
const char *use_movft = is_direct ? "movt:" : "movf:";
fprintf(out, "    %-12s%s <- %s , 0\n", use_movft, t, v);
            rtl_v0_live = false;
            return t;
        }

        /* ── float binary ── */
        if (is_float) {
            const char *op = rtl_binop_float(lbl);
            const char *f_l_reg = nullptr;
            const char *f_r_reg = nullptr;

            if (llf && rlf) {
                char *f_l   = rtl_alloc_f_front();
                char *f_res = rtl_alloc_f_front();
                char *f_r   = rtl_alloc_f_front();
                rtl_load_into(node->child1, f_l, out);
                rtl_load_into(node->child2, f_r, out);
                rtl_free_f(f_l); rtl_free_f(f_r);
                fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", f_res, f_l, f_r);
                return f_res;
            }
            else if (llf && !rlf) {
                f_r_reg = rtl_gen_expr(node->child2, out);
                char *f_l = rtl_alloc_f_front();
                rtl_load_into(node->child1, f_l, out);
                f_l_reg = f_l;
                char *f_res = rtl_alloc_f_front();
                rtl_free_f(f_l_reg); rtl_free_f(f_r_reg);
                fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", f_res, f_l_reg, f_r_reg);
                return f_res;
            }
            else if (!llf && rlf) {
                f_l_reg = rtl_gen_expr(node->child1, out);
                char *f_r = rtl_alloc_f_right_leaf();
                rtl_load_into(node->child2, f_r, out);
                f_r_reg = f_r;
                char *f_res = rtl_alloc_f_front();
                rtl_free_f(f_l_reg); rtl_free_f(f_r_reg);
                fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", f_res, f_l_reg, f_r_reg);
                return f_res;
            }
            else {
                f_l_reg = rtl_gen_expr(node->child1, out);
                f_r_reg = rtl_gen_expr(node->child2, out);
                char *f_res = rtl_alloc_f_front();
                rtl_free_f(f_l_reg); rtl_free_f(f_r_reg);
                fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", f_res, f_l_reg, f_r_reg);
                return f_res;
            }
        }

        /* ── int binary ── */
        const char *op = rtl_binop(lbl);
        const char *regL, *regR;

        if (llf && rlf) {
            if (!rtl_v0_live) {
                char *t_res = rtl_alloc_t_min();
                char *v     = rtl_get_v0();
                char *t_r   = rtl_alloc_t_min();
                rtl_load_into(node->child1, v,   out);
                rtl_load_into(node->child2, t_r, out);
                rtl_free_reg(v); rtl_free_reg(t_r);
                fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t_res, v, t_r);
                return t_res;
            } else {
                char *t_l   = rtl_alloc_t_min();
                char *t_res = rtl_alloc_t_min();
                char *t_r   = rtl_alloc_t_min();
                rtl_load_into(node->child1, t_l,  out);
                rtl_load_into(node->child2, t_r,  out);
                rtl_free_reg(t_l); rtl_free_reg(t_r);
                fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t_res, t_l, t_r);
                return t_res;
            }
        }
        else if (llf && !rlf) {
            regR = rtl_gen_expr(node->child2, out);
            char *lreg = rtl_alloc_v0_or_t();
            rtl_load_into(node->child1, lreg, out);
            regL = lreg;
            char *t_res = rtl_alloc_t_min();
            rtl_free_reg(regL); rtl_free_reg(regR);
            fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t_res, regL, regR);
            return t_res;
        }
        else if (!llf && rlf) {
            regL = rtl_gen_expr(node->child1, out);
            if (regL[0] == 't') {
                if (!rtl_v0_live) {
                    char *t_r = rtl_alloc_t_min();
                    rtl_load_into(node->child2, t_r, out);
                    regR = t_r;
                    char *v = rtl_get_v0();
                    rtl_free_reg(regL); rtl_free_reg(regR);
                    fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", v, regL, regR);
                    return v;
                } else {
                    char *t_r = rtl_alloc_t_max();
                    rtl_load_into(node->child2, t_r, out);
                    regR = t_r;
                    char *t_res = rtl_alloc_t_min();
                    rtl_free_reg(regL); rtl_free_reg(regR);
                    fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t_res, regL, regR);
                    return t_res;
                }
            } else {
                char *t_res = rtl_alloc_t_min();
                char *t_r   = rtl_alloc_t_min();
                rtl_load_into(node->child2, t_r, out);
                regR = t_r;
                rtl_free_reg(regL); rtl_free_reg(regR);
                fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t_res, regL, regR);
                return t_res;
            }
        }
        else {
            regL = rtl_gen_expr(node->child1, out);
            regR = rtl_gen_expr(node->child2, out);
            const char *result;
            if (regL[0] == 't' && regR[0] == 't') {
                if (!rtl_v0_live) {
                    char *v = rtl_get_v0();
                    rtl_free_reg(regL); rtl_free_reg(regR);
                    fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", v, regL, regR);
                    result = v;
                } else {
                    char *t = rtl_alloc_t_min();
                    rtl_free_reg(regL); rtl_free_reg(regR);
                    fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t, regL, regR);
                    result = t;
                }
            } else {
                char *t = rtl_alloc_t_min();
                rtl_free_reg(regL); rtl_free_reg(regR);
                fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t, regL, regR);
                result = t;
            }
            return result;
        }
    }

    /* ── ternary ?: ── */
    if (strcmp(lbl, "Condition: ?:") == 0)
    {
        const char *cond = rtl_gen_expr(node->child1, out);

        char stemp[32];
        sprintf(stemp, "stemp%d", rtl_stemp_cnt++);
        char *Lfalse = rtl_new_label();
        char *Lend   = rtl_new_label();

        char *not_r;
        if (cond[0] == 't' && !rtl_v0_live)
            not_r = rtl_get_v0();
        else
            not_r = rtl_alloc_t();
        rtl_free_reg(cond);
        fprintf(out, "    %-12s%s <- %s\n", "not:", not_r, cond);
        fprintf(out, "    %-12s%s , %s\n",  "bgtz:", not_r, Lfalse);
        rtl_free_reg(not_r);

        int ternary_is_float = (strcmp(node_type(node->child2), "float") == 0);

        /* true branch */
        rtl_reset_regs();
        const char *tr = rtl_gen_expr(node->child2, out);
        if (ternary_is_float) {
            fprintf(out, "    %-12s%s <- %s\n", "store.d:", stemp, tr);
            rtl_free_f(tr);
        } else {
            fprintf(out, "    %-12s%s <- %s\n", "store:", stemp, tr);
            rtl_free_reg(tr);
        }
        fprintf(out, "    %-12s%s\n", "goto:", Lend);

        /* false branch */
        fprintf(out, "\n  %s:\n", Lfalse); free(Lfalse);
        rtl_reset_regs();
        const char *fr = rtl_gen_expr(node->child3, out);
        if (ternary_is_float) {
            fprintf(out, "    %-12s%s <- %s\n", "store.d:", stemp, fr);
            rtl_free_f(fr);
        } else {
            fprintf(out, "    %-12s%s <- %s\n", "store:", stemp, fr);
            rtl_free_reg(fr);
        }

        /* end label + load stemp */
        fprintf(out, "\n  %s:\n", Lend); free(Lend);
        rtl_reset_regs();
        if (ternary_is_float) {
            char *f = rtl_alloc_f_front();
            fprintf(out, "    %-12s%s <- %-10s;; Loading variable %s into register\n",
                    "load.d:", f, stemp, stemp);
            return f;
        } else {
            fprintf(out, "    %-12sv0 <- %-10s;; Loading variable %s into register\n",
                    "load:", stemp, stemp);
            return rtl_get_v0();
        }
    }

    char *v = rtl_get_v0();
    return v;
}

/* ════════════════════════════════════════════
   WRITE HELPER
   ════════════════════════════════════════════ */
static void rtl_gen_write(const char *val_lbl, ASTNode *expr, FILE *out)
{
    const char *lbl = expr ? expr->label : val_lbl;

    if (strncmp(lbl, "Str:", 4) == 0) {
        char buf[256]; rtl_str(lbl, buf);
        int idx = rtl_add_string(buf);
        char sname[32]; sprintf(sname, "_str_%d", idx);
        char *v = rtl_get_v0();
        char *a = rtl_alloc_a();
        fprintf(out, "    %-12s%s <- %-10s;; Loading 4 in %s to indicate syscall to print string value\n",
                "iLoad:", v, "4", v);
        fprintf(out, "    %-12s%s <- %-10s;; String = %s\n",
                "load_addr:", a, sname, buf);
        fprintf(out, "    write                       ;; This is where syscall will be made\n");
        rtl_free_reg(v); rtl_free_reg(a);

    } else if (strncmp(lbl, "Name:", 5) == 0) {
        char buf[128]; rtl_name(lbl, buf);
        int is_str   = (strstr(lbl, "<string>") != NULL);
        int is_float = (strstr(lbl, "<float>")  != NULL);
        if (is_float) {
            char *v = rtl_get_v0();
            fprintf(out, "    %-12s%s <- %-10s;; Loading 3 in %s to indicate syscall to print double value\n",
                    "iLoad:", v, "3", v);
            fprintf(out, "    %-12sf12 <- %-10s;; Moving the value to be printed into register f12\n",
                    "load.d:", buf);
            fprintf(out, "    write                       ;; This is where syscall will be made\n");
            rtl_free_reg(v);
        } else if (is_str) {
            char *v = rtl_get_v0();
            char *a = rtl_alloc_a();
            fprintf(out, "    %-12s%s <- %-10s;; Loading 4 in %s to indicate syscall to print string value\n",
                    "iLoad:", v, "4", v);
            fprintf(out, "    %-12s%s <- %-10s;; Moving the value to be printed into register %s\n",
                    "load:", a, buf, a);
            fprintf(out, "    write                       ;; This is where syscall will be made\n");
            rtl_free_reg(v); rtl_free_reg(a);
        } else {
            char *v = rtl_get_v0();
            char *a = rtl_alloc_a();
            fprintf(out, "    %-12s%s <- %-10s;; Loading 1 in %s to indicate syscall to print integer value\n",
                    "iLoad:", v, "1", v);
            fprintf(out, "    %-12s%s <- %-10s;; Moving the value to be printed into register %s\n",
                    "load:", a, buf, a);
            fprintf(out, "    write                       ;; This is where syscall will be made\n");
            rtl_free_reg(v); rtl_free_reg(a);
        }

    } else if (strncmp(lbl, "Num:", 4) == 0) {
        char buf[128]; rtl_num(lbl, buf);
        int is_float = (strstr(lbl, "<float>") != NULL);
        if (is_float) {
            double val = atof(buf);
            char *v = rtl_get_v0();
            fprintf(out, "    %-12s%s <- %-10s;; Loading 3 in %s to indicate syscall to print double value\n",
                    "iLoad:", v, "3", v);
            fprintf(out, "    %-12sf12 <- %-10s;; Loading float number %f\n",
                    "iLoad.d:", buf, val);
            fprintf(out, "    write                       ;; This is where syscall will be made\n");
            rtl_free_reg(v);
        } else {
            char *v = rtl_get_v0();
            char *a = rtl_alloc_a();
            fprintf(out, "    %-12s%s <- %-10s;; Loading 1 in %s to indicate syscall to print integer value\n",
                    "iLoad:", v, "1", v);
            fprintf(out, "    %-12s%s <- %-10s;; Moving the value to be printed into register %s\n",
                    "iLoad:", a, buf, a);
            fprintf(out, "    write                       ;; This is where syscall will be made\n");
            rtl_free_reg(v); rtl_free_reg(a);
        }

    } else if (expr && !is_leaf(expr)) {
        const char *r = rtl_gen_expr(expr, out);
        char *v = rtl_get_v0();
        char *a = rtl_alloc_a();
        fprintf(out, "    %-12s%s <- %-10s;; Loading 1 in %s to indicate syscall to print integer value\n",
                "iLoad:", v, "1", v);
        fprintf(out, "    %-12s%s <- %-10s;; Moving the value to be printed into register %s\n",
                "load:", a, r, a);
        fprintf(out, "    write                       ;; This is where syscall will be made\n");
        rtl_free_reg(r); rtl_free_reg(v); rtl_free_reg(a);
    }
}

/* ════════════════════════════════════════════
   STATEMENT GENERATOR
   ════════════════════════════════════════════ */
static void rtl_gen_stmt(ASTNode *node, FILE *out)
{
    while (node) {
        const char *lbl = node->label;

        rtl_reset_regs();

        /* assignment */
        if (strcmp(lbl, "Asgn:") == 0) {
            char dest[128]; rtl_name(node->child1->label, dest);
            int dest_is_float = (strstr(node->child1->label, "<float>") != NULL);
            ASTNode *rhs = node->child2;
            if (is_leaf(rhs) && strncmp(rhs->label, "Str:", 4) == 0) {
                char buf[256]; rtl_str(rhs->label, buf);
                int idx = rtl_add_string(buf);
                char sname[32]; sprintf(sname, "_str_%d", idx);
                char *v = rtl_get_v0();
                fprintf(out, "    %-12s%s <- %-10s;; String = %s\n",
                        "load_addr:", v, sname, buf);
                fprintf(out, "    %-12s%s <- %s\n", "store:", dest, v);
                rtl_free_reg(v);
            } else if (is_leaf(rhs) && strncmp(rhs->label, "FuncCall:", 9) != 0) {
                if (dest_is_float) {
                    char *f = rtl_alloc_f_front();
                    rtl_load_into(rhs, f, out);
                    fprintf(out, "    %-12s%s <- %s\n", "store.d:", dest, f);
                    rtl_free_f(f);
                } else {
                    char *v = rtl_get_v0();
                    rtl_load_into(rhs, v, out);
                    fprintf(out, "    %-12s%s <- %s\n", "store:", dest, v);
                    rtl_free_reg(v);
                }
            } else {
                const char *r = rtl_gen_expr(rhs, out);
                /* handle function call result — need move before store */
                if (strcmp(r, "v1") == 0) {
                    char *v = rtl_get_v0();
                    fprintf(out, "    %-12s%s <- %s\n", "move:", v, r);
                    fprintf(out, "    %-12s%s <- %s\n", "store:", dest, v);
                    rtl_free_reg(v);
                } else if (strcmp(r, "f0") == 0) {
                    char *f = rtl_alloc_f_front();
                    fprintf(out, "    %-12s%s <- %s\n", "move.d:", f, r);
                    if (dest_is_float) {
                        fprintf(out, "    %-12s%s <- %s\n", "store.d:", dest, f);
                    } else {
                        fprintf(out, "    %-12s%s <- %s\n", "store:", dest, f);
                    }
                    rtl_free_f(f);
                } else if (dest_is_float) {
                    fprintf(out, "    %-12s%s <- %s\n", "store.d:", dest, r);
                    rtl_free_f(r);
                } else {
                    fprintf(out, "    %-12s%s <- %s\n", "store:", dest, r);
                    rtl_free_reg(r);
                }
            }
        }

        /* read */
        else if (strncmp(lbl, "Read:", 5) == 0) {
            const char *p = strstr(lbl, ": Name : ");
            if (p) p += 9; else p = lbl + 6;
            char tmp[64]; strcpy(tmp, p);
            char *cut = strchr(tmp, '<'); if (cut) *cut = '\0';
            int read_is_float = (strstr(lbl, "<float>") != NULL);
            char *v = rtl_get_v0();
            if (read_is_float) {
                fprintf(out, "    %-12s%s <- %-10s;; Loading 7 in %s to indicate syscall to read double value\n",
                        "iLoad:", v, "7", v);
                fprintf(out, "    read                        ;; This is where syscall will be made\n");
                fprintf(out, "    %-12s%s <- f0          ;; Moving the read value from register f0\n",
                        "store.d:", tmp);
            } else {
                fprintf(out, "    %-12s%s <- %-10s;; Loading 5 in %s to indicate syscall to read integer value\n",
                        "iLoad:", v, "5", v);
                fprintf(out, "    read                        ;; This is where syscall will be made\n");
                fprintf(out, "    %-12s%s <- %s          ;; Moving the read value from register %s\n",
                        "store:", tmp, v, v);
            }
            rtl_free_reg(v);
        }

        /* write */
        else if (strncmp(lbl, "Write:", 6) == 0) {
            if (node->child1) rtl_gen_write(NULL, node->child1, out);
            else              rtl_gen_write(lbl + 7, NULL, out);
        }

        /* if / if-else */
        else if (strcmp(lbl, "If:") == 0)
        {
            const char *cond = rtl_gen_expr(node->child1, out);

            char *tbuf=NULL; size_t tsz=0;
            FILE *tout=open_memstream(&tbuf,&tsz);
            if (node->child2) rtl_gen_stmt(node->child2, tout);
            fclose(tout);

            char *Lend  = rtl_new_label();
            char *Lelse = node->child3 ? rtl_new_label() : strdup(Lend);

            char *not_r;
            if (cond[0] == 't' && !rtl_v0_live)
                not_r = rtl_get_v0();
            else
                not_r = rtl_alloc_t();
            rtl_free_reg(cond);
            fprintf(out, "    %-12s%s <- %s\n", "not:", not_r, cond);
            fprintf(out, "    %-12s%s , %s\n",  "bgtz:", not_r, Lelse);
            rtl_free_reg(not_r);

            fprintf(out, "%s", tbuf); free(tbuf);
            fprintf(out, "    %-12s%s\n", "goto:", Lend);
            if (strcmp(Lelse, Lend) != 0) fprintf(out, "\n  %s:\n", Lelse);
            free(Lelse);
            if (node->child3) rtl_gen_stmt(node->child3, out);
            fprintf(out, "\n  %s:\n", Lend); free(Lend);
        }

        /* while */
        else if (strcmp(lbl, "While:") == 0)
        {
            char *cbuf=NULL; size_t csz=0;
            FILE *cout=open_memstream(&cbuf,&csz);
            const char *cond = rtl_gen_expr(node->child1, cout);
            fclose(cout);

            char *bbuf=NULL; size_t bsz=0;
            FILE *bout=open_memstream(&bbuf,&bsz);
            if (node->child2) rtl_gen_stmt(node->child2, bout);
            fclose(bout);

            char *Lstart = rtl_new_label();
            char *Lend   = rtl_new_label();

            fprintf(out, "\n  %s:\n", Lstart);
            fprintf(out, "%s", cbuf); free(cbuf);

            char *not_r;
            if (cond[0] == 't' && !rtl_v0_live)
                not_r = rtl_get_v0();
            else
                not_r = rtl_alloc_t();
            rtl_free_reg(cond);
            fprintf(out, "    %-12s%s <- %s\n", "not:", not_r, cond);
            fprintf(out, "    %-12s%s , %s\n",  "bgtz:", not_r, Lend);
            rtl_free_reg(not_r);

            fprintf(out, "%s", bbuf); free(bbuf);
            fprintf(out, "    %-12s%s\n\n", "goto:", Lstart);
            fprintf(out, "  %s:\n", Lend);
            free(Lstart); free(Lend);
        }

        /* do-while */
        else if (strcmp(lbl, "Do:") == 0)
        {
            char *bbuf=NULL; size_t bsz=0;
            FILE *bout=open_memstream(&bbuf,&bsz);
            if (node->child1) rtl_gen_stmt(node->child1, bout);
            fclose(bout);

            char *Lstart = rtl_new_label();

            fprintf(out, "\n  %s:\n", Lstart);
            fprintf(out, "%s", bbuf); free(bbuf);

            const char *r = rtl_gen_expr(node->child2, out);
            fprintf(out, "    %-12s%s , %s\n", "bgtz:", r, Lstart);
            rtl_free_reg(r);
            free(Lstart);
        }

        /* return */
        else if (strncmp(lbl, "Return:", 7) == 0) {
            int ret_is_float = (strcmp(rtl_current_func_ret_type, "float") == 0);
            if (node->child1) {
                rtl_reset_regs();
                if (is_leaf(node->child1)) {
                    if (ret_is_float) {
                        char *f = rtl_alloc_f_front();
                        rtl_load_into(node->child1, f, out);
                        fprintf(out, "    %-12s%s <- %s\n", "store.d:", rtl_ret_stemp, f);
                        rtl_free_f(f);
                    } else {
                        char *v = rtl_get_v0();
                        rtl_load_into(node->child1, v, out);
                        fprintf(out, "    %-12s%s <- %s\n", "store:", rtl_ret_stemp, v);
                        rtl_free_reg(v);
                    }
                } else {
                    const char *r = rtl_gen_expr(node->child1, out);
                    if (ret_is_float) {
                        fprintf(out, "    %-12s%s <- %s\n", "store.d:", rtl_ret_stemp, r);
                        rtl_free_f(r);
                    } else {
                        fprintf(out, "    %-12s%s <- %s\n", "store:", rtl_ret_stemp, r);
                        rtl_free_reg(r);
                    }
                }
                fprintf(out, "    %-12s%s\n", "goto:", rtl_ret_label);
                rtl_has_return = 1;
            } else {
                fprintf(out, "    return\n");
            }
        }

        /* function call as statement */
        else if (strncmp(lbl, "FuncCall:", 9) == 0) {
            char fname[64];
            const char *p = lbl + 10;
            const char *cut = strchr(p, '<');
            if (cut) { int n=cut-p; strncpy(fname,p,n); fname[n]='\0'; }
            else strcpy(fname, p);

            const char *ret = func_lookup(fname);
            if (ret && strcmp(ret, "void") == 0) {
                fprintf(out, "    call %s_\n", fname);
            } else {
                /* call but discard result */
                const char *r = rtl_gen_expr(node, out);
                rtl_free_reg(r);
            }
        }

        node = node->next;
    }
}

/* ════════════════════════════════════════════
   ENTRY POINT
   ════════════════════════════════════════════ */
inline void print_rtl(FILE *out)
{
    int indices[MAX_FUNCS];
    int count = 0;
    for (int i = 0; i < func_count; i++) {
        if (func_table[i].defined)
            indices[count++] = i;
    }

    /* generation order: sort by raw name */
    int gen_order[MAX_FUNCS];
    for (int i = 0; i < count; i++) gen_order[i] = indices[i];
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(func_table[gen_order[i]].name, func_table[gen_order[j]].name) > 0) {
                int tmp = gen_order[i]; gen_order[i] = gen_order[j]; gen_order[j] = tmp;
            }
        }
    }

    rtl_label_count = 0;
    rtl_str_count   = 0;
    rtl_stemp_cnt   = 0;
    rtl_str_table.clear();

    /* PASS 1: pre-allocate return labels in insertion order */
    int ret_label_nums[MAX_FUNCS];
    for (int i = 0; i < MAX_FUNCS; i++) ret_label_nums[i] = -1;
    for (int i = 0; i < func_count; i++) {
        if (func_table[i].defined && strcmp(func_table[i].ret_type, "void") != 0) {
            ret_label_nums[i] = rtl_label_count++;
        }
    }

    /* Pre-register strings in insertion order */
    for (int i = 0; i < func_count; i++) {
        if (!func_table[i].defined || !func_table[i].body) continue;
        ASTNode *n = func_table[i].body;
        while (n) {
            if (strncmp(n->label, "Str:", 4) == 0) {
                char buf[256]; rtl_str(n->label, buf);
                rtl_add_string(buf);
            }
            /* check assignment RHS for string literals */
            if (strcmp(n->label, "Asgn:") == 0 && n->child2 &&
                strncmp(n->child2->label, "Str:", 4) == 0) {
                char buf[256]; rtl_str(n->child2->label, buf);
                rtl_add_string(buf);
            }
            n = n->next;
        }
    }

    /* PASS 2: generate RTL in raw-name order, buffer each */
    char *bufs[MAX_FUNCS] = {NULL};
    for (int k = 0; k < count; k++) {
        int idx = gen_order[k];
        FuncSymbol *fn = &func_table[idx];

        /* restore locals for this function */
        memcpy(local_table, fn->local_syms, fn->local_sym_count * sizeof(Symbol));
        local_count = fn->local_sym_count;

        rtl_reset_regs();
        rtl_stemp_cnt = 0;
        rtl_has_return = 0;
        rtl_current_func_ret_type = fn->ret_type;

        int is_non_void = (strcmp(fn->ret_type, "void") != 0);
        if (is_non_void) {
            sprintf(rtl_ret_stemp, "stemp%d", rtl_stemp_cnt++);
            sprintf(rtl_ret_label, "Label%d", ret_label_nums[idx]);
        }

        char *body_buf = NULL; size_t body_sz = 0;
        FILE *body_mem = open_memstream(&body_buf, &body_sz);
        if (fn->body) rtl_gen_stmt(fn->body, body_mem);
        if (is_non_void && rtl_has_return) {
            int ret_is_float = (strcmp(fn->ret_type, "float") == 0);
            fprintf(body_mem, "\n%s:\n", rtl_ret_label);
            if (ret_is_float) {
                fprintf(body_mem, "    %-12sf0 <- %-10s;; Moving the value to be printed into register f0\n",
                        "load.d:", rtl_ret_stemp);
                fprintf(body_mem, "    return      f0\n");
            } else {
                fprintf(body_mem, "    %-12sv1 <- %-10s;; Moving the value to be printed into register v1\n",
                        "load:", rtl_ret_stemp);
                fprintf(body_mem, "    return      v1\n");
            }
        }
        fclose(body_mem);

        if (body_sz == 0) { free(body_buf); continue; }

        char *buf = NULL; size_t sz = 0;
        FILE *mem = open_memstream(&buf, &sz);
        if (strcmp(fn->name, "main") == 0)
            fprintf(mem, "**PROCEDURE: %s\n", fn->name);
        else
            fprintf(mem, "**PROCEDURE: %s_\n", fn->name);
        fprintf(mem, "**BEGIN: RTL Statements\n");
        fprintf(mem, "%s", body_buf);
        fprintf(mem, "**END: RTL Statements\n");
        fclose(mem);
        free(body_buf);
        bufs[idx] = buf;
    }

    /* print order: sort by name_ */
    int print_order[MAX_FUNCS];
    for (int i = 0; i < count; i++) print_order[i] = indices[i];
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            char a[128], b[128];
            sprintf(a, "%s_", func_table[print_order[i]].name);
            sprintf(b, "%s_", func_table[print_order[j]].name);
            if (strcmp(a, b) > 0) {
                int tmp = print_order[i]; print_order[i] = print_order[j]; print_order[j] = tmp;
            }
        }
    }

    for (int k = 0; k < count; k++) {
        int idx = print_order[k];
        if (bufs[idx]) {
            fprintf(out, "%s", bufs[idx]);
            free(bufs[idx]);
        }
    }
}

#endif