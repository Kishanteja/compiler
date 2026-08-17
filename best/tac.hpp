#ifndef TAC_HPP
#define TAC_HPP

#include "ast.hpp"

static int tac_temp  = 0;
static int tac_stemp = 0;
static int tac_label = 0;

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
    /* step 1: condition first */
    char *cond = tac_gen_expr(node->child1, out);

    /* step 2: buffer only then branch */
    char *tbuf=NULL; size_t tsz=0;
    FILE *tout=open_memstream(&tbuf,&tsz);
    if (node->child2) tac_gen_stmt(node->child2, tout);
    fclose(tout);

    /* step 3: labels and negation after buffering */
    char *Lend  = tac_new_label();
    char *Lelse = node->child3 ? tac_new_label() : strdup(Lend);
    char *neg   = tac_new_temp();

    /* step 4: emit */
    fprintf(out, "    %s = !  %s\n", neg, cond); free(cond);
    fprintf(out, "    if(%s) goto %s\n", neg, Lelse); free(neg);
    fprintf(out, "%s", tbuf); free(tbuf);
    fprintf(out, "    goto %s\n", Lend);

    /* only print Lelse if different from Lend */
    if (strcmp(Lelse, Lend) != 0)
        fprintf(out, "%s:\n", Lelse);
    free(Lelse);

    if (node->child3) tac_gen_stmt(node->child3, out);
    fprintf(out, "%s:\n", Lend); free(Lend);
}

        /* ── while ── */
        else if (strcmp(lbl, "While:") == 0)
        {
            /* step 1: buffer condition */
            char *cbuf=NULL; size_t csz=0;
            FILE *cout=open_memstream(&cbuf,&csz);
            char *cond=tac_gen_expr(node->child1,cout);
            fclose(cout);

            /* step 2: buffer body (if labels allocated here get lower numbers) */
            char *bbuf=NULL; size_t bsz=0;
            FILE *bout=open_memstream(&bbuf,&bsz);
            if (node->child2) tac_gen_stmt(node->child2,bout);
            fclose(bout);

            /* step 3: while labels AFTER body */
            char *Lstart=tac_new_label();
            char *Lend  =tac_new_label();

            /* step 4: negation temp last */
            char *neg=tac_new_temp();

            /* step 5: emit */
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
    /* step 1: buffer body first — if labels inside get lower numbers */
    char *bbuf=NULL; size_t bsz=0;
    FILE *bout=open_memstream(&bbuf,&bsz);
    if (node->child1) tac_gen_stmt(node->child1, bout);
    fclose(bout);

    /* step 2: allocate do-while label AFTER body */
    char *Lstart = tac_new_label();   /* gets higher number */

    /* step 3: emit */
    fprintf(out, "%s:\n", Lstart);
    fprintf(out, "%s", bbuf); free(bbuf);

    /* step 4: condition — positive, no negation temp */
    char *cond = tac_gen_expr(node->child2, out);
    fprintf(out, "    if(%s) goto %s\n", cond, Lstart); free(cond);

    free(Lstart);
}

        node = node->next;
    }
}

inline void print_tac(FILE *out)
{
    if (!root) return;
    tac_temp = tac_stemp = tac_label = 0;
    fprintf(out,"**PROCEDURE: main\n");
    fprintf(out,"**BEGIN: Three Address Code Statements\n");
    tac_gen_stmt(root, out);
    fprintf(out,"**END: Three Address Code Statements\n");
}

#endif