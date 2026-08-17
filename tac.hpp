#ifndef TAC_HPP
#define TAC_HPP

#include "ast.hpp"
#include "symtab.hpp"

static int tac_temp  = 0;
static int tac_stemp = 0;
static int tac_label = 0;

static char tac_ret_stemp[16];
static char tac_ret_label[16];
static int  tac_has_return = 0;

static char *tac_new_temp(void) { char *b=(char*)malloc(16); sprintf(b,"temp%d",tac_temp++); return b; }
static char *tac_new_stemp(void){ char *b=(char*)malloc(16); sprintf(b,"stemp%d",tac_stemp++); return b; }
static char *tac_new_label(void){ char *b=(char*)malloc(16); sprintf(b,"Label%d",tac_label++); return b; }

static char *tac_leaf_val(const char *lbl)
{
    char *buf = (char*)malloc(128);
    const char *p, *cut;
    if (strncmp(lbl, "Name:", 5) == 0) {
        p = lbl+6; cut = strchr(p,'<');
        if (cut) { int n=cut-p; strncpy(buf,p,n); buf[n]='\0'; } else strcpy(buf,p);
    } else if (strncmp(lbl, "Num:", 4) == 0) {
        p = lbl+5; cut = strchr(p,'<');
        if (cut) { int n=cut-p; strncpy(buf,p,n); buf[n]='\0'; } else strcpy(buf,p);
    } else if (strncmp(lbl, "Str:", 4) == 0) {
        p = lbl+5; cut = strrchr(p,'<');
        if (cut) { int n=cut-p; strncpy(buf,p,n); buf[n]='\0'; } else strcpy(buf,p);
    } else if (strncmp(lbl, "Bool:", 5) == 0) {
        p = lbl+6; cut = strchr(p,'<');
        if (cut) { int n=cut-p; strncpy(buf,p,n); buf[n]='\0'; } else strcpy(buf,p);
    } else { strcpy(buf,lbl); }
    return buf;
}

static const char *tac_binop(const char *lbl)
{
    if (strstr(lbl,"Plus"))  return "+";
    if (strstr(lbl,"Minus")) return "-";
    if (strstr(lbl,"Mult"))  return "*";
    if (strstr(lbl,"Div"))   return "/";
    if (strstr(lbl,": GT"))  return ">";
    if (strstr(lbl,": LT"))  return "<";
    if (strstr(lbl,": GE"))  return ">=";
    if (strstr(lbl,": LE"))  return "<=";
    if (strstr(lbl,": EQ"))  return "==";
    if (strstr(lbl,": NE"))  return "!=";
    if (strstr(lbl,": AND")) return "&&";
    if (strstr(lbl,": OR"))  return "||";
    return "?";
}

static char *tac_gen_expr(ASTNode *node, FILE *out);
static void  tac_gen_stmt(ASTNode *node, FILE *out);

static char *tac_gen_expr(ASTNode *node, FILE *out)
{
    if (!node) return strdup("?");
    const char *lbl = node->label;

    if (strncmp(lbl, "FuncCall:", 9) == 0) {
        char fname[64];
        const char *p = lbl + 10;
        const char *cut = strchr(p, '<');
        if (cut) { int n=cut-p; strncpy(fname,p,n); fname[n]='\0'; }
        else strcpy(fname, p);

        /* allocate result temp before args so numbering matches reference */
        char *t = tac_new_temp();

        char *args[MAX_PARAMS];
        int argc = 0;
        ASTNode *arg = node->child1;
        while (arg) {
            ASTNode *next_arg = arg->next;
            arg->next = NULL;
            args[argc++] = tac_gen_expr(arg, out);
            arg->next = next_arg;
            arg = next_arg;
        }

        fprintf(out, "    %s = %s_(", t, fname);
        for (int i = 0; i < argc; i++) {
            if (i > 0) fprintf(out, ", ");
            fprintf(out, "%s", args[i]);
            free(args[i]);
        }
        fprintf(out, ")\n");
        return t;
    }

    if (!node->child1 && !node->child2 && !node->child3)
        return tac_leaf_val(lbl);

    if (strncmp(lbl, "Arith: Uminus", 13) == 0) {
        char *op=tac_gen_expr(node->child1,out); char *t=tac_new_temp();
        fprintf(out,"    %s = -%s\n",t,op); free(op); return t;
    }
    if (strncmp(lbl, "Condition: NOT", 14) == 0) {
        char *op=tac_gen_expr(node->child1,out); char *t=tac_new_temp();
        fprintf(out,"    %s = !  %s\n",t,op); free(op); return t;
    }
    if (node->child1 && node->child2 && !node->child3) {
        char *l=tac_gen_expr(node->child1,out);
        char *r=tac_gen_expr(node->child2,out);
        char *t=tac_new_temp();
        fprintf(out,"    %s = %s %s %s\n",t,l,tac_binop(lbl),r);
        free(l); free(r); return t;
    }
    if (strcmp(lbl, "Condition: ?:") == 0)
    {
        char *cond   = tac_gen_expr(node->child1, out);
        char *t      = tac_new_stemp();
        char *Lfalse = tac_new_label();
        char *Lend   = tac_new_label();

        char *tbuf=NULL; size_t tsz=0;
        char *fbuf=NULL; size_t fsz=0;
        FILE *tout=open_memstream(&tbuf,&tsz);
        FILE *fout=open_memstream(&fbuf,&fsz);

        char *tv=tac_gen_expr(node->child2,tout);
        fprintf(tout,"    %s = %s\n",t,tv); free(tv); fclose(tout);

        char *fv=tac_gen_expr(node->child3,fout);
        fprintf(fout,"    %s = %s\n",t,fv); free(fv); fclose(fout);

        char *neg=tac_new_temp();
        fprintf(out,"    %s = !  %s\n",neg,cond); free(cond);
        fprintf(out,"    if(%s) goto %s\n",neg,Lfalse); free(neg);
        fprintf(out,"%s",tbuf); free(tbuf);
        fprintf(out,"    goto %s\n",Lend);
        fprintf(out,"%s:\n",Lfalse); free(Lfalse);
        fprintf(out,"%s",fbuf); free(fbuf);
        fprintf(out,"%s:\n",Lend); free(Lend);
        return strdup(t);
    }
    return strdup("?");
}

static void tac_gen_stmt(ASTNode *node, FILE *out)
{
    while (node) {
        const char *lbl = node->label;
        if (strcmp(lbl,"Asgn:") == 0) {
            char *dest=tac_leaf_val(node->child1->label);
            char *rhs=tac_gen_expr(node->child2,out);
            fprintf(out,"    %s = %s\n",dest,rhs);
            free(dest); free(rhs);
        } else if (strncmp(lbl,"Read:",5) == 0) {
            const char *p=strstr(lbl,": Name : ");
            if (p) p+=9; else p=lbl+6;
            char tmp[64]; strcpy(tmp,p);
            char *cut=strchr(tmp,'<');
            if (cut) *cut='\0';
            fprintf(out,"    read %s\n",tmp);
        } else if (strncmp(lbl,"Write:",6) == 0) {
            if (node->child1) {
                char *v=tac_gen_expr(node->child1,out);
                fprintf(out,"    write %s\n",v); free(v);
            } else {
                char *v=tac_leaf_val(lbl+7);
                fprintf(out,"    write %s\n",v); free(v);
            }
        }
        
        else if (strcmp(lbl, "If:") == 0)
        {
            char *cond = tac_gen_expr(node->child1, out);

            char *tbuf=NULL; size_t tsz=0;
            FILE *tout=open_memstream(&tbuf,&tsz);
            if (node->child2) tac_gen_stmt(node->child2, tout);
            fclose(tout);

            char *Lend  = tac_new_label();
            char *Lelse = node->child3 ? tac_new_label() : strdup(Lend);
            char *neg   = tac_new_temp();

            fprintf(out, "    %s = !  %s\n", neg, cond); free(cond);
            fprintf(out, "    if(%s) goto %s\n", neg, Lelse); free(neg);
            fprintf(out, "%s", tbuf); free(tbuf);
            fprintf(out, "    goto %s\n", Lend);

            if (strcmp(Lelse, Lend) != 0)
                fprintf(out, "%s:\n", Lelse);
            free(Lelse);

            if (node->child3) tac_gen_stmt(node->child3, out);
            fprintf(out, "%s:\n", Lend); free(Lend);
        }

        /* ── while ── */
        else if (strcmp(lbl, "While:") == 0)
        {
            char *cbuf=NULL; size_t csz=0;
            FILE *cout=open_memstream(&cbuf,&csz);
            char *cond=tac_gen_expr(node->child1,cout);
            fclose(cout);

            char *bbuf=NULL; size_t bsz=0;
            FILE *bout=open_memstream(&bbuf,&bsz);
            if (node->child2) tac_gen_stmt(node->child2,bout);
            fclose(bout);

            char *Lstart=tac_new_label();
            char *Lend  =tac_new_label();

            char *neg=tac_new_temp();

            fprintf(out,"%s:\n",Lstart);
            fprintf(out,"%s",cbuf); free(cbuf);
            fprintf(out,"    %s = !  %s\n",neg,cond); free(cond);
            fprintf(out,"    if(%s) goto %s\n",neg,Lend); free(neg);
            fprintf(out,"%s",bbuf); free(bbuf);
            fprintf(out,"    goto %s\n",Lstart);
            fprintf(out,"%s:\n",Lend);
            free(Lstart); free(Lend);
        }

        /* ── do-while ── */
        else if (strcmp(lbl, "Do:") == 0)
        {
            char *bbuf=NULL; size_t bsz=0;
            FILE *bout=open_memstream(&bbuf,&bsz);
            if (node->child1) tac_gen_stmt(node->child1, bout);
            fclose(bout);

            char *Lstart = tac_new_label();

            fprintf(out, "%s:\n", Lstart);
            fprintf(out, "%s", bbuf); free(bbuf);

            char *cond = tac_gen_expr(node->child2, out);
            fprintf(out, "    if(%s) goto %s\n", cond, Lstart); free(cond);

            free(Lstart);
        }

        /* ── return ── */
        else if (strncmp(lbl, "Return:", 7) == 0) {
            if (node->child1) {
                char *v = tac_gen_expr(node->child1, out);
                fprintf(out, "    %s = %s\n", tac_ret_stemp, v);
                fprintf(out, "    goto %s\n", tac_ret_label);
                free(v);
                tac_has_return = 1;
            } else {
                fprintf(out, "    return\n");
            }
        }

        /* ── function call as statement ── */
        else if (strncmp(lbl, "FuncCall:", 9) == 0) {
            char fname[64];
            const char *p = lbl + 10;
            const char *cut = strchr(p, '<');
            if (cut) { int n=cut-p; strncpy(fname,p,n); fname[n]='\0'; }
            else strcpy(fname, p);

            char *args[MAX_PARAMS];
            int argc = 0;
            ASTNode *arg = node->child1;
            while (arg) {
                ASTNode *next_arg = arg->next;
                arg->next = NULL;
                args[argc++] = tac_gen_expr(arg, out);
                arg->next = next_arg;
                arg = next_arg;
            }

            fprintf(out, "    %s_(", fname);
            for (int i = 0; i < argc; i++) {
                if (i > 0) fprintf(out, ", ");
                fprintf(out, "%s", args[i]);
                free(args[i]);
            }
            fprintf(out, ")\n");
        }

        node = node->next;
    }
}

inline void print_tac(FILE *out)
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

    tac_label = 0;

    /* PASS 1: pre-allocate return labels in INSERTION order (func_table order) */
    int ret_label_nums[MAX_FUNCS];
    for (int i = 0; i < MAX_FUNCS; i++) ret_label_nums[i] = -1;
    for (int i = 0; i < func_count; i++) {
        if (func_table[i].defined && strcmp(func_table[i].ret_type, "void") != 0) {
            ret_label_nums[i] = tac_label++;
        }
    }

    /* PASS 2: generate TAC bodies */
    char *bufs[MAX_FUNCS] = {NULL};
    for (int k = 0; k < count; k++) {
        int idx = gen_order[k];
        FuncSymbol *fn = &func_table[idx];
        tac_stemp = 0;
        tac_temp  = 0;
        tac_has_return = 0;

        int is_non_void = (strcmp(fn->ret_type, "void") != 0);
        if (is_non_void) {
            sprintf(tac_ret_stemp, "stemp%d", tac_stemp++);
            sprintf(tac_ret_label, "Label%d", ret_label_nums[idx]);
        }

        char *body_buf = NULL; size_t body_sz = 0;
        FILE *body_mem = open_memstream(&body_buf, &body_sz);
        if (fn->body) tac_gen_stmt(fn->body, body_mem);
        if (is_non_void && tac_has_return) {
            fprintf(body_mem, "%s:\n", tac_ret_label);
            fprintf(body_mem, "    return %s\n", tac_ret_stemp);
        }
        fclose(body_mem);

        if (body_sz == 0) { free(body_buf); continue; }

        char *buf = NULL; size_t sz = 0;
        FILE *mem = open_memstream(&buf, &sz);
        if (strcmp(fn->name, "main") == 0)
            fprintf(mem, "**PROCEDURE: %s\n", fn->name);
        else
            fprintf(mem, "**PROCEDURE: %s_\n", fn->name);
        fprintf(mem, "**BEGIN: Three Address Code Statements\n");
        fprintf(mem, "%s", body_buf);
        fprintf(mem, "**END: Three Address Code Statements\n");
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