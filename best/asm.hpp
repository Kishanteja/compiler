#ifndef ASM_HPP
#define ASM_HPP

#include "ast.hpp"
#include "symtab.hpp"
#include "rtl.hpp"
#include <string>
#include <vector>
#include <set>
#include <map>

static std::map<std::string, int> asm_local_offset;
static int asm_frame_size = 0;
static int asm_label_count = 0;
static int asm_stemp_cnt   = 0;
static char asm_current_func[128];
static char asm_ret_label[16];
static int  asm_stemp_offset = 0;
static int  asm_has_return = 0;
static const char *asm_current_ret_type = "void";

static char *asm_new_label(void) {
    char *b = (char*)malloc(16);
    sprintf(b, "L%d", asm_label_count++);
    return b;
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
    for (int i = 0; i < fn->local_sym_count; i++) {
        bool is_param = false;
        for (int p = 0; p < fn->param_count; p++)
            if (strcmp(fn->local_syms[i].name, fn->param_names[p]) == 0) { is_param = true; break; }
        if (is_param) continue;
        char buf[128]; sprintf(buf, "%s_", fn->local_syms[i].name);
        if (strcmp(fn->local_syms[i].type, "float") == 0) { neg_size += 8; asm_local_offset[std::string(buf)] = -neg_size; }
        else { neg_size += 4; asm_local_offset[std::string(buf)] = -neg_size; }
    }
    asm_stemp_offset = 0;
    if (strcmp(fn->ret_type, "void") != 0) {
        if (strcmp(fn->ret_type, "float") == 0) { neg_size += 8; asm_stemp_offset = -neg_size; }
        else { neg_size += 4; asm_stemp_offset = -neg_size; }
    }
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

static void asm_load_into(ASTNode *node, const char *reg, FILE *out) {
    const char *lbl = node->label; char buf[256];
    if (strncmp(lbl, "Name:", 5) == 0) {
        rtl_name(lbl, buf);
        if (strstr(lbl, "<float>")) asm_load_float_var(buf, reg, out);
        else asm_load_var(buf, reg, out);
    } else if (strncmp(lbl, "Num:", 4) == 0) {
        rtl_num(lbl, buf);
        if (strstr(lbl, "<float>")) { double v=atof(buf); fprintf(out, "\tli.d $%s, %.2f\t\t\t# Source:%.6f\n", reg, v, v); }
        else fprintf(out, "\tli $%s, %s\t\t\t# Source:%s\n", reg, buf, buf);
    } else if (strncmp(lbl, "Str:", 4) == 0) {
        char buf2[256]; rtl_str(lbl, buf2); int idx=rtl_add_string(buf2);
        char sn[32]; sprintf(sn,"_str_%d",idx);
        fprintf(out, "\tla $%s, %s\t\t\t# String = %s\n", reg, sn, buf2);
    } else if (strncmp(lbl, "Bool:", 5) == 0) {
        fprintf(out, "\tli $%s, %s\n", reg, strstr(lbl,"true")?"1":"0");
    }
}

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

static void asm_gen_stmt(ASTNode *node, FILE *out);

/* Returns register name holding result */
static const char *asm_gen_expr(ASTNode *node, FILE *out) {
    if (!node) return "v0";
    const char *lbl = node->label;
    int is_float = (strcmp(node_type(node), "float") == 0);

    /* function call */
    if (strncmp(lbl, "FuncCall:", 9) == 0) {
        char fname[64]; const char *p=lbl+10; const char *cut=strchr(p,'<');
        if (cut) { int n=cut-p; strncpy(fname,p,n); fname[n]='\0'; } else strcpy(fname,p);
        int is_float_ret = (strcmp(node_type(node), "float") == 0);
        int argc=0; ASTNode *args_arr[MAX_PARAMS]; ASTNode *arg=node->child1;
        while (arg) { args_arr[argc++]=arg; arg=arg->next; }
        if (argc > 0) {
            for (int i=argc-1; i>=0; i--) {
                ASTNode *ns=args_arr[i]->next; args_arr[i]->next=NULL;
                int af=(strcmp(node_type(args_arr[i]),"float")==0);
                if (is_leaf(args_arr[i]) && strncmp(args_arr[i]->label,"FuncCall:",9)!=0) {
                    if (af) { asm_load_into(args_arr[i],"f2",out);
                        fprintf(out,"\ts.d $f2, -4($sp)\t\t\t# store formal parameter at sp\n");
                        fprintf(out,"\tsub $sp, $sp, 8\t\t\t# decrement the stack pointer by 8\n");
                    } else { asm_load_into(args_arr[i],"v0",out);
                        fprintf(out,"\tsw $v0, 0($sp)\t\t\t# store formal parameter at sp\n");
                        fprintf(out,"\tsub $sp, $sp, 4\t\t\t# decrement the stack pointer by 4\n");
                    }
                } else {
                    const char *r=asm_gen_expr(args_arr[i],out);
                    if (af) { fprintf(out,"\ts.d $%s, -4($sp)\t\t\t# store formal parameter at sp\n",r);
                        fprintf(out,"\tsub $sp, $sp, 8\t\t\t# decrement the stack pointer by 8\n");
                    } else { fprintf(out,"\tsw $%s, 0($sp)\t\t\t# store formal parameter at sp\n",r);
                        fprintf(out,"\tsub $sp, $sp, 4\t\t\t# decrement the stack pointer by 4\n");
                    }
                }
                args_arr[i]->next=ns;
            }
        }
        fprintf(out,"\tjal %s_\n",fname);
        for (int i=0;i<argc;i++) {
            int af=(strcmp(node_type(args_arr[i]),"float")==0);
            fprintf(out,"\tadd $sp, $sp, %d\t\t\t# %s the stack pointer by %d\n",
                af?8:4, af?"increment":"add", af?8:4);
        }
        return is_float_ret ? "f0" : "v1";
    }

    /* leaf */
    if (!node->child1 && !node->child2 && !node->child3) {
        if (is_float) { asm_load_into(node,"f2",out); return "f2"; }
        else { asm_load_into(node,"v0",out); return "v0"; }
    }

    /* uminus */
    if (strncmp(lbl,"Arith: Uminus",13)==0) {
        const char *r=asm_gen_expr(node->child1,out);
        if (is_float) { fprintf(out,"\tneg.d $f0, $%s\n",r); return "f0"; }
        else { fprintf(out,"\tneg $v0, $%s\n",r); return "v0"; }
    }

    /* NOT */
    if (strncmp(lbl,"Condition: NOT",14)==0) {
        const char *r=asm_gen_expr(node->child1,out);
        fprintf(out,"\tseq $v0, $%s, $zero\n",r); return "v0";
    }

    /* binary */
    if (node->child1 && node->child2 && !node->child3) {
        int child_is_float=(strcmp(node_type(node->child1),"float")==0);
        bool llf=is_leaf(node->child1)&&strncmp(node->child1->label,"FuncCall:",9)!=0;
        bool rlf=is_leaf(node->child2)&&strncmp(node->child2->label,"FuncCall:",9)!=0;

        /* float comparison */
        if (!is_float && child_is_float) {
            const char *lr=asm_gen_expr(node->child1,out);
            fprintf(out,"\tmov.d $f2, $%s\n",lr);
            const char *rr=asm_gen_expr(node->child2,out);
            fprintf(out,"\tmov.d $f4, $%s\n",rr);
            if (strstr(lbl,": GT")) fprintf(out,"\tc.le.d $f2, $f4\n\tli $v0, 1\n\tmovt $v0, $zero, 0\n");
            else if (strstr(lbl,": GE")) fprintf(out,"\tc.lt.d $f2, $f4\n\tli $v0, 1\n\tmovt $v0, $zero, 0\n");
            else if (strstr(lbl,": LT")) fprintf(out,"\tc.lt.d $f2, $f4\n\tli $v0, 0\n\tmovt $v0, $zero+1, 0\n");
            else if (strstr(lbl,": LE")) fprintf(out,"\tc.le.d $f2, $f4\n\tli $v0, 0\n\tmovt $v0, $zero+1, 0\n");
            else if (strstr(lbl,": EQ")) fprintf(out,"\tc.eq.d $f2, $f4\n\tli $v0, 0\n\tmovt $v0, $zero+1, 0\n");
            else if (strstr(lbl,": NE")) fprintf(out,"\tc.eq.d $f2, $f4\n\tli $v0, 1\n\tmovt $v0, $zero, 0\n");
            return "v0";
        }

        /* float arith */
        if (is_float) {
            const char *op=asm_mips_op_float(lbl);
            if (llf&&rlf) {
                asm_load_into(node->child1,"f2",out);
                asm_load_into(node->child2,"f6",out);
                fprintf(out,"\t%s $f4, $f2, $f6\t# Result: $f4, Opd1: $f2, Opd2:$f6\n",op);
                return "f4";
            } else if (!llf&&rlf) {
                const char *lr=asm_gen_expr(node->child1,out);
                if (strcmp(lr,"f2")!=0) fprintf(out,"\tmov.d $f2, $%s\n",lr);
                asm_load_into(node->child2,"f6",out);
                fprintf(out,"\t%s $f4, $f2, $f6\t# Result: $f4, Opd1: $f2, Opd2:$f6\n",op);
                return "f4";
            } else if (llf&&!rlf) {
                const char *rr=asm_gen_expr(node->child2,out);
                if (strcmp(rr,"f6")!=0) fprintf(out,"\tmov.d $f6, $%s\n",rr);
                asm_load_into(node->child1,"f2",out);
                fprintf(out,"\t%s $f4, $f2, $f6\t# Result: $f4, Opd1: $f2, Opd2:$f6\n",op);
                return "f4";
            } else {
                const char *lr=asm_gen_expr(node->child1,out);
                fprintf(out,"\tmov.d $f2, $%s\n",lr);
                const char *rr=asm_gen_expr(node->child2,out);
                fprintf(out,"\tmov.d $f6, $%s\n",rr);
                fprintf(out,"\t%s $f4, $f2, $f6\t# Result: $f4, Opd1: $f2, Opd2:$f6\n",op);
                return "f4";
            }
        }

        /* int arith/comparison */
        const char *op=asm_mips_op(lbl);
        if (llf&&rlf) {
            asm_load_into(node->child1,"v0",out);
            asm_load_into(node->child2,"t1",out);
            if (strcmp(op,"div")==0) { fprintf(out,"\tdiv $v0, $t1\n\tmflo $t0\n"); }
            else fprintf(out,"\t%s $t0, $v0, $t1\t# Result: $t0, Opd1: $v0, Opd2:$t1\n",op);
            return "t0";
        } else if (!llf&&rlf) {
            const char *lr=asm_gen_expr(node->child1,out);
            asm_load_into(node->child2,"t1",out);
            if (strcmp(lr,"v0")==0) {
                if (strcmp(op,"div")==0) { fprintf(out,"\tdiv $v0, $t1\n\tmflo $t0\n"); }
                else fprintf(out,"\t%s $t0, $v0, $t1\t# Result: $t0, Opd1: $v0, Opd2:$t1\n",op);
                return "t0";
            } else {
                if (strcmp(op,"div")==0) { fprintf(out,"\tdiv $t0, $t1\n\tmflo $v0\n"); }
                else fprintf(out,"\t%s $v0, $t0, $t1\t# Result: $v0, Opd1: $t0, Opd2:$t1\n",op);
                return "v0";
            }
        } else if (llf&&!rlf) {
            const char *rr=asm_gen_expr(node->child2,out);
            fprintf(out,"\tmove $t1, $%s\n",rr);
            asm_load_into(node->child1,"v0",out);
            if (strcmp(op,"div")==0) { fprintf(out,"\tdiv $v0, $t1\n\tmflo $t0\n"); }
            else fprintf(out,"\t%s $t0, $v0, $t1\t# Result: $t0, Opd1: $v0, Opd2:$t1\n",op);
            return "t0";
        } else {
            const char *lr=asm_gen_expr(node->child1,out);
            fprintf(out,"\tmove $t0, $%s\n",lr);
            const char *rr=asm_gen_expr(node->child2,out);
            fprintf(out,"\tmove $t1, $%s\n",rr);
            if (strcmp(op,"div")==0) { fprintf(out,"\tdiv $t0, $t1\n\tmflo $v0\n"); }
            else fprintf(out,"\t%s $v0, $t0, $t1\t# Result: $v0, Opd1: $t0, Opd2:$t1\n",op);
            return "v0";
        }
    }

    /* ternary */
    if (strcmp(lbl,"Condition: ?:")==0) {
        char stemp[32]; sprintf(stemp,"stemp%d",asm_stemp_cnt++);
        char *Lf=asm_new_label(); char *Le=asm_new_label();
        int tf=(strcmp(node_type(node->child2),"float")==0);
        const char *cr=asm_gen_expr(node->child1,out);
        fprintf(out,"\tbeqz $%s, %s\n",cr,Lf);
        const char *tr=asm_gen_expr(node->child2,out);
        if (tf) fprintf(out,"\ts.d $%s, %s_data\n",tr,stemp);
        else fprintf(out,"\tsw $%s, %s_data\n",tr,stemp);
        fprintf(out,"\tj %s\n",Le);
        fprintf(out,"%s:\n",Lf);
        const char *fr=asm_gen_expr(node->child3,out);
        if (tf) fprintf(out,"\ts.d $%s, %s_data\n",fr,stemp);
        else fprintf(out,"\tsw $%s, %s_data\n",fr,stemp);
        fprintf(out,"%s:\n",Le);
        free(Lf); free(Le);
        if (tf) { fprintf(out,"\tl.d $f0, %s_data\n",stemp); return "f0"; }
        else { fprintf(out,"\tlw $v0, %s_data\n",stemp); return "v0"; }
    }
    return "v0";
}

static void asm_gen_write(const char *val_lbl, ASTNode *expr, FILE *out) {
    const char *lbl = expr ? expr->label : val_lbl;
    if (strncmp(lbl,"Str:",4)==0) {
        char buf[256]; rtl_str(lbl,buf); int idx=rtl_add_string(buf);
        char sn[32]; sprintf(sn,"_str_%d",idx);
        fprintf(out,"\tli $v0, 4\t\t\t# Loading 4 in v0 to indicate syscall to print string value\n");
        fprintf(out,"\tla $a0, %s\t\t\t# String = %s\n",sn,buf);
        fprintf(out,"\tsyscall\n");
    } else if (strncmp(lbl,"Name:",5)==0) {
        char buf[128]; rtl_name(lbl,buf);
        int is_str=(strstr(lbl,"<string>")!=NULL); int is_f=(strstr(lbl,"<float>")!=NULL);
        if (is_f) { fprintf(out,"\tli $v0, 3\t\t\t# Loading 3 in v0 to indicate syscall to print double value\n");
            asm_load_float_var(buf,"f12",out); fprintf(out,"\tsyscall\n");
        } else if (is_str) { fprintf(out,"\tli $v0, 4\t\t\t# Loading 4 in v0 to indicate syscall to print string value\n");
            asm_load_var(buf,"a0",out); fprintf(out,"\tsyscall\n");
        } else { fprintf(out,"\tli $v0, 1\t\t\t# Loading 1 in v0 to indicate syscall to print integer value\n");
            asm_load_var(buf,"a0",out); fprintf(out,"\tsyscall\n");
        }
    } else if (strncmp(lbl,"Num:",4)==0) {
        char buf[128]; rtl_num(lbl,buf); int is_f=(strstr(lbl,"<float>")!=NULL);
        if (is_f) { double v=atof(buf);
            fprintf(out,"\tli $v0, 3\t\t\t# Loading 3 in v0 to indicate syscall to print double value\n");
            fprintf(out,"\tli.d $f12, %.6f\t\t\t# Loading float number %.6f\n",v,v);
            fprintf(out,"\tsyscall\n");
        } else { fprintf(out,"\tli $v0, 1\t\t\t# Loading 1 in v0 to indicate syscall to print integer value\n");
            fprintf(out,"\tli $a0, %s\t\t\t# Moving the value to be printed into register a0\n",buf);
            fprintf(out,"\tsyscall\n");
        }
    } else if (expr && !is_leaf(expr)) {
        const char *r=asm_gen_expr(expr,out);
        fprintf(out,"\tli $v0, 1\t\t\t# Loading 1 in v0 to indicate syscall to print integer value\n");
        fprintf(out,"\tmove $a0, $%s\t\t\t# Moving the value to be printed into register a0\n",r);
        fprintf(out,"\tsyscall\n");
    }
}

static void asm_gen_stmt(ASTNode *node, FILE *out) {
    while (node) {
        const char *lbl = node->label;
        if (strcmp(lbl,"Asgn:")==0) {
            char dest[128]; rtl_name(node->child1->label, dest);
            int df=(strstr(node->child1->label,"<float>")!=NULL);
            ASTNode *rhs=node->child2;
            if (is_leaf(rhs)&&strncmp(rhs->label,"Str:",4)==0) {
                char buf[256]; rtl_str(rhs->label,buf); int idx=rtl_add_string(buf);
                char sn[32]; sprintf(sn,"_str_%d",idx);
                fprintf(out,"\tla $v0, %s\t\t\t# String = %s\n",sn,buf);
                asm_store_var(dest,"v0",out);
            } else if (is_leaf(rhs)&&strncmp(rhs->label,"FuncCall:",9)!=0) {
                if (df) { asm_load_into(rhs,"f2",out); asm_store_float_var(dest,"f2",out); }
                else { asm_load_into(rhs,"v0",out); asm_store_var(dest,"v0",out); }
            } else {
                const char *r=asm_gen_expr(rhs,out);
                int rfc=(strncmp(rhs->label,"FuncCall:",9)==0);
                int rff=rfc&&(strcmp(node_type(rhs),"float")==0);
                if (rfc&&rff) { fprintf(out,"\tmov.d $f2, $f0\t\t\t# store function call return in $f0 so that another function call can be made\n"); asm_store_float_var(dest,"f2",out); }
                else if (rfc) { fprintf(out,"\tmove $v0, $v1\t\t\t# store function call return in $v1 so that another function call can be made\n"); asm_store_var(dest,"v0",out); }
                else if (df) asm_store_float_var(dest,r,out);
                else asm_store_var(dest,r,out);
            }
        }
        else if (strncmp(lbl,"Read:",5)==0) {
            const char *p=strstr(lbl,": Name : "); if(p) p+=9; else p=lbl+6;
            char tmp[64]; strcpy(tmp,p); char *cut=strchr(tmp,'<'); if(cut)*cut='\0';
            if (strstr(lbl,"<float>")) { fprintf(out,"\tli $v0, 7\t\t\t# Loading 7 in v0 to indicate syscall to read double value\n");
                fprintf(out,"\tsyscall\n"); asm_store_float_var(tmp,"f0",out);
            } else { fprintf(out,"\tli $v0, 5\t\t\t# Loading 5 in v0 to indicate syscall to read integer value\n");
                fprintf(out,"\tsyscall\n"); asm_store_var(tmp,"v0",out);
            }
        }
        else if (strncmp(lbl,"Write:",6)==0) {
            if (node->child1) asm_gen_write(NULL,node->child1,out);
            else asm_gen_write(lbl+7,NULL,out);
        }
        else if (strcmp(lbl,"If:")==0) {
            const char *cr=asm_gen_expr(node->child1,out);
            char *Le=asm_new_label(); char *Lel=node->child3?asm_new_label():strdup(Le);
            fprintf(out,"\tbeqz $%s, %s\n",cr,Lel);
            if (node->child2) asm_gen_stmt(node->child2,out);
            if (node->child3) { fprintf(out,"\tj %s\n",Le); fprintf(out,"%s:\n",Lel); asm_gen_stmt(node->child3,out); }
            fprintf(out,"%s:\n",Le); free(Lel); free(Le);
        }
        else if (strcmp(lbl,"While:")==0) {
            char *Ls=asm_new_label(); char *Le=asm_new_label();
            fprintf(out,"%s:\n",Ls);
            const char *cr=asm_gen_expr(node->child1,out);
            fprintf(out,"\tbeqz $%s, %s\n",cr,Le);
            if (node->child2) asm_gen_stmt(node->child2,out);
            fprintf(out,"\tj %s\n",Ls); fprintf(out,"%s:\n",Le); free(Ls); free(Le);
        }
        else if (strcmp(lbl,"Do:")==0) {
            char *Ls=asm_new_label(); fprintf(out,"%s:\n",Ls);
            if (node->child1) asm_gen_stmt(node->child1,out);
            const char *cr=asm_gen_expr(node->child2,out);
            fprintf(out,"\tbnez $%s, %s\n",cr,Ls); free(Ls);
        }
        else if (strncmp(lbl,"Return:",7)==0) {
            int rf=(strcmp(asm_current_ret_type,"float")==0);
            if (node->child1) {
                const char *r=asm_gen_expr(node->child1,out);
                if (rf) fprintf(out,"\ts.d $%s, %d($fp)\t\t\t# Dest: stemp0\n",r,asm_stemp_offset);
                else fprintf(out,"\tsw $%s, %d($fp)\t\t\t# Dest: stemp0\n",r,asm_stemp_offset);
                fprintf(out,"\tj %s\n",asm_ret_label);
                asm_has_return=1;
            } else fprintf(out,"\tj epilogue_%s\n",asm_current_func);
        }
        else if (strncmp(lbl,"FuncCall:",9)==0) { asm_gen_expr(node,out); }
        node=node->next;
    }
}

inline void print_asm(FILE *out) {
    int indices[MAX_FUNCS]; int count=0;
    for (int i=0;i<func_count;i++) if (func_table[i].defined) indices[count++]=i;
    int gen_order[MAX_FUNCS];
    for (int i=0;i<count;i++) gen_order[i]=indices[i];
    for (int i=0;i<count-1;i++) for (int j=i+1;j<count;j++)
        if (strcmp(func_table[gen_order[i]].name,func_table[gen_order[j]].name)>0)
            { int t=gen_order[i]; gen_order[i]=gen_order[j]; gen_order[j]=t; }

    rtl_str_count=0; rtl_str_table.clear(); asm_label_count=0; asm_stemp_cnt=0;

    for (int i=0;i<func_count;i++) {
        if (!func_table[i].defined||!func_table[i].body) continue;
        ASTNode *n=func_table[i].body;
        while(n) {
            if (strncmp(n->label,"Str:",4)==0) { char buf[256]; rtl_str(n->label,buf); rtl_add_string(buf); }
            if (strcmp(n->label,"Asgn:")==0&&n->child2&&strncmp(n->child2->label,"Str:",4)==0)
                { char buf[256]; rtl_str(n->child2->label,buf); rtl_add_string(buf); }
            n=n->next;
        }
    }

    int ret_label_nums[MAX_FUNCS];
    for (int i=0;i<MAX_FUNCS;i++) ret_label_nums[i]=-1;
    for (int i=0;i<func_count;i++)
        if (func_table[i].defined&&strcmp(func_table[i].ret_type,"void")!=0)
            ret_label_nums[i]=asm_label_count++;

    fprintf(out,"\n    .data\n");
    for (int i=0;i<sym_count;i++) {
        if (strcmp(sym_table[i].type,"float")==0) fprintf(out,"%s_:\t.double 0.0\n",sym_table[i].name);
        else if (strcmp(sym_table[i].type,"int")==0||strcmp(sym_table[i].type,"bool")==0) fprintf(out,"%s_:\t.word 0\n",sym_table[i].name);
        else if (strcmp(sym_table[i].type,"string")==0) fprintf(out,"%s_:\t.word 0\n",sym_table[i].name);
    }

    char *bufs[MAX_FUNCS]={NULL};
    for (int k=0;k<count;k++) {
        int idx=gen_order[k]; FuncSymbol *fn=&func_table[idx];
        memcpy(local_table,fn->local_syms,fn->local_sym_count*sizeof(Symbol));
        local_count=fn->local_sym_count;
        asm_collect_locals_for_func(fn);
        int nv=(strcmp(fn->ret_type,"void")!=0);
        asm_has_return=0; asm_current_ret_type=fn->ret_type;
        char fl[128];
        if (strcmp(fn->name,"main")==0) strcpy(fl,"main"); else sprintf(fl,"%s_",fn->name);
        strcpy(asm_current_func,fl);
        if (nv) sprintf(asm_ret_label,"Label%d",ret_label_nums[idx]);

        char *buf=NULL; size_t sz=0; FILE *mem=open_memstream(&buf,&sz);
        fprintf(mem,"    .text\t\t\t# The .text assembler directive indicates\n");
        fprintf(mem,"    .globl %s\t\t\t# The following is the code (as opposed to data)\n",fl);
        fprintf(mem,"%s:\t\t\t\t# .globl makes main know to the outside of the program.\n",fl);
        fprintf(mem,"# Prologue begins\n");
        fprintf(mem,"    sw $ra, 0($sp)\t\t\t# Save the return address\n");
        fprintf(mem,"    sw $fp, -4($sp)\t\t\t# Save the frame pointer\n");
        fprintf(mem,"    sub $fp, $sp, 4\t\t\t# Update the frame pointer\n");
        fprintf(mem,"    sub $sp, $sp, %d\t\t\t# Make space for the locals\n",asm_frame_size);
        fprintf(mem,"# Prologue ends\n\n");
        if (fn->body) asm_gen_stmt(fn->body,mem);
        if (nv&&asm_has_return) {
            int rf=(strcmp(fn->ret_type,"float")==0);
            fprintf(mem,"%s:\n",asm_ret_label);
            if (rf) fprintf(mem,"    l.d $f0, %d($fp)\t\t\t# Moving the value to be printed into register a0\n",asm_stemp_offset);
            else fprintf(mem,"    lw $v1, %d($fp)\t\t\t# Moving the value to be printed into register a0\n",asm_stemp_offset);
            fprintf(mem,"    j epilogue_%s\n",fl);
        }
        fprintf(mem,"epilogue_%s:\n",fl);
        fprintf(mem,"    add $sp, $sp, %d\t\t\t# Increment stack pointer for local variables\n",asm_frame_size);
        fprintf(mem,"    lw $fp, -4($sp)\t\t\t# Set $fp to $sp-4\n");
        fprintf(mem,"    lw $ra, 0($sp)\t\t\t# Save ra\n");
        fprintf(mem,"    jr $ra\t\t\t\t# Jump back to the called procedure\n");
        fprintf(mem,"# Epilogue Ends\n");
        fclose(mem); bufs[idx]=buf;
    }

    int po[MAX_FUNCS];
    for (int i=0;i<count;i++) po[i]=indices[i];
    for (int i=0;i<count-1;i++) for (int j=i+1;j<count;j++) {
        char a[128],b[128]; sprintf(a,"%s_",func_table[po[i]].name); sprintf(b,"%s_",func_table[po[j]].name);
        if (strcmp(a,b)>0) { int t=po[i]; po[i]=po[j]; po[j]=t; }
    }
    for (int k=0;k<count;k++) {
        int idx=po[k];
        if (bufs[idx]) { fprintf(out,"%s",bufs[idx]); if(k<count-1) fprintf(out,"\n"); free(bufs[idx]); }
    }
    if (!rtl_str_table.empty()) {
        fprintf(out,"\n\t.data\n");
        for (int i=0;i<(int)rtl_str_table.size();i++)
            fprintf(out,"_str_%d:\t.asciiz %s\n",i,rtl_str_table[i].c_str());
    }
}

#endif