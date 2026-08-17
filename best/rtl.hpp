#ifndef RTL_HPP
#define RTL_HPP

#include "ast.hpp"
#include <string>
#include <vector>

/* ════════════════════════════════════════════
   STRING TABLE & COUNTERS
   ════════════════════════════════════════════ */
static std::vector<std::string> rtl_str_table;
static int rtl_str_count   = 0;
static int rtl_label_count = 0;

/* ── dynamic register allocator ── */
static int rtl_t_max = 0;
static int rtl_v_max = 0;
static int rtl_a_max = 0;
static std::vector<int> rtl_t_free;
static std::vector<int> rtl_v_free;
static std::vector<int> rtl_a_free;

static char *rtl_alloc_reg(const char *prefix, int &maxn, std::vector<int> &fl)
{
    char *b = (char*)malloc(8);
    int n;
    if (!fl.empty()) { n = fl.back(); fl.pop_back(); }
    else              { n = maxn++; }
    sprintf(b, "%s%d", prefix, n);
    return b;
}

static char *rtl_alloc_t() { return rtl_alloc_reg("t", rtl_t_max, rtl_t_free); }
static char *rtl_alloc_v() { return rtl_alloc_reg("v", rtl_v_max, rtl_v_free); }
static char *rtl_alloc_a() { return rtl_alloc_reg("a", rtl_a_max, rtl_a_free); }

static void rtl_free_reg(const char *reg)
{
    if (!reg) return;
    if      (reg[0] == 't') rtl_t_free.push_back(atoi(reg+1));
    else if (reg[0] == 'v') rtl_v_free.push_back(atoi(reg+1));
    else if (reg[0] == 'a') rtl_a_free.push_back(atoi(reg+1));
}

static void rtl_reset_regs()
{
    rtl_t_max = 0; rtl_t_free.clear();
    rtl_v_max = 0; rtl_v_free.clear();
    rtl_a_max = 0; rtl_a_free.clear();
}

static int rtl_add_string(const char *s)
{
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
        /* variable load — use load.d with f2 */
        fprintf(out, "    %-12sf2 <- %-10s;; Loading variable %s into register\n",
                "load.d:", buf, buf);
    } else {
        fprintf(out, "    %-12s%s <- %-10s;; Loading variable %s into register\n",
                "load:", reg, buf, buf);
    }
} else if (strncmp(lbl, "Num:", 4) == 0) {
    rtl_num(lbl, buf);
    int is_float = (strstr(lbl, "<float>") != NULL);
    if (is_float) {
        /* float literal — use iLoad.d with raw value */
        double val = atof(buf);
        fprintf(out, "    %-12sf2 <- %-10s;; Loading float number %f\n",
                "iLoad.d:", buf, val);
    } else {
        fprintf(out, "    %-12s%s <- %-10s;; Loading integer number %s\n",
                "iLoad:", reg, buf, buf);
    }
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

static void rtl_gen_stmt(ASTNode *node, FILE *out);

/* ════════════════════════════════════════════
   EXPRESSION GENERATOR
   ════════════════════════════════════════════ */
static const char *rtl_gen_expr(ASTNode *node, FILE *out)
{
    if (!node) return "v0";
    const char *lbl = node->label;

    /* ── leaf: alloc v, load into it ── */
    if (!node->child1 && !node->child2 && !node->child3) {
    char *reg;
    if (rtl_v_max == 0 || !rtl_v_free.empty())
        reg = rtl_alloc_v();
    else
        reg = rtl_alloc_t();
    rtl_load_into(node, reg, out);
    return reg;
}

    /* ── unary minus: result always t ── */
    /* unary minus */
if (strncmp(lbl, "Arith: Uminus", 13) == 0) {
    const char *r = rtl_gen_expr(node->child1, out);
    char *res;
    if (r[0] == 't' && (rtl_v_max == 0 || !rtl_v_free.empty())) {
        /* input is t and v0 is free → result to v0 */
        res = rtl_alloc_v();
    } else {
        /* v0 occupied → result to t */
        res = rtl_alloc_t();
    }
    rtl_free_reg(r);
    fprintf(out, "    %-12s%s <- %s\n", "uminus:", res, r);
    return res;
}

    /* ── logical NOT: result always t ── */
    /* logical NOT */
if (strncmp(lbl, "Condition: NOT", 14) == 0) {
    const char *r = rtl_gen_expr(node->child1, out);
    char *res;
    if (r[0] == 't' && (rtl_v_max == 0 || !rtl_v_free.empty())) {
        res = rtl_alloc_v();
    } else {
        res = rtl_alloc_t();
    }
    rtl_free_reg(r);
    fprintf(out, "    %-12s%s <- %s\n", "not:", res, r);
    return res;
}

    /* ── binary op ── */
    if (node->child1 && node->child2 && !node->child3) {
    const char *op = rtl_binop(lbl);
    bool llf = is_leaf(node->child1);
    bool rlf = is_leaf(node->child2);
    const char *regL, *regR;

    if (llf && rlf) {
        /* both leaves: pre-alloc result first → t0, left → v0, right → t1 */
        char *t_res = rtl_alloc_t();
        char *v     = rtl_alloc_v();
        char *t_r   = rtl_alloc_t();
        rtl_load_into(node->child1, v,   out);
        rtl_load_into(node->child2, t_r, out);
        rtl_free_reg(v); rtl_free_reg(t_r);
        fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t_res, v, t_r);
        return t_res;
    }
    else if (llf && !rlf) {
    /* left=leaf, right=expr: generate right first, then load left,
       allocate result AFTER operands (not before) */
    regR = rtl_gen_expr(node->child2, out);
    char *lreg;
    if (rtl_v_max == 0 || !rtl_v_free.empty())
        lreg = rtl_alloc_v();
    else
        lreg = rtl_alloc_t();
    rtl_load_into(node->child1, lreg, out);
    regL = lreg;
    /* allocate result AFTER both operands are done */
    char *t_res = rtl_alloc_t();
    rtl_free_reg(regL); rtl_free_reg(regR);
    fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t_res, regL, regR);
    return t_res;
}
    else if (!llf && rlf) {
        /* left=expr, right=leaf: generate left, force right into t */
        regL = rtl_gen_expr(node->child1, out);
        char *t_r = rtl_alloc_t();
        rtl_load_into(node->child2, t_r, out);
        regR = t_r;
        const char *result;
        if (regL[0] == 't' && regR[0] == 't') {
            char *v = rtl_alloc_v();
            rtl_free_reg(regL); rtl_free_reg(regR);
            fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", v, regL, regR);
            result = v;
        } else {
            char *t = rtl_alloc_t();
            rtl_free_reg(regL); rtl_free_reg(regR);
            fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", t, regL, regR);
            result = t;
        }
        return result;
    }
    else {
        /* both non-leaf */
        regL = rtl_gen_expr(node->child1, out);
        regR = rtl_gen_expr(node->child2, out);
        const char *result;
        if (regL[0] == 't' && regR[0] == 't') {
            char *v = rtl_alloc_v();
            rtl_free_reg(regL); rtl_free_reg(regR);
            fprintf(out, "    %s:%-8s%s <- %s , %s\n", op, "", v, regL, regR);
            result = v;
        } else {
            char *t = rtl_alloc_t();
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
        /* step 1: condition */
        const char *cond = rtl_gen_expr(node->child1, out);

        /* step 2: buffer branches */
        char *tbuf=NULL; size_t tsz=0;
        char *fbuf=NULL; size_t fsz=0;
        FILE *tout=open_memstream(&tbuf,&tsz);
        FILE *fout=open_memstream(&fbuf,&fsz);
        rtl_gen_expr(node->child2, tout); fclose(tout);
        rtl_gen_expr(node->child3, fout); fclose(fout);

        /* step 3: labels after buffering */
        char *Lfalse = rtl_new_label();
        char *Lend   = rtl_new_label();

        /* step 4: NOT cond */
        char *not_r = rtl_alloc_t();
        rtl_free_reg(cond);
        fprintf(out, "    %-12s%s <- %s\n", "not:", not_r, cond);
        fprintf(out, "    %-12s%s , %s\n",  "bgtz:", not_r, Lfalse);
        rtl_free_reg(not_r);

        fprintf(out, "%s", tbuf); free(tbuf);
        fprintf(out, "    %-12s%s\n", "goto:", Lend);
        fprintf(out, "\n  %s:\n", Lfalse); free(Lfalse);
        fprintf(out, "%s", fbuf); free(fbuf);
        fprintf(out, "\n  %s:\n", Lend); free(Lend);

        char *v = rtl_alloc_v();
        return v;
    }

    return rtl_alloc_v();
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
        char *v = rtl_alloc_v();
        char *a = rtl_alloc_a();
        fprintf(out, "    %-12s%s <- %-10s;; Loading 4 in %s to indicate syscall to print string value\n",
                "iLoad:", v, "4", v);
        fprintf(out, "    %-12s%s <- %-10s;; Moving the value to be printed into register %s\n",
                "load_addr:", a, sname, a);
        fprintf(out, "    write                       ;; This is where syscall will be made\n");
        rtl_free_reg(v); rtl_free_reg(a);

    } else if (strncmp(lbl, "Name:", 5) == 0) {
    char buf[128]; rtl_name(lbl, buf);
    int is_str   = (strstr(lbl, "<string>") != NULL);
    int is_float = (strstr(lbl, "<float>")  != NULL);
    char *v = rtl_alloc_v();
        char *a = rtl_alloc_a();

    if (is_float) {
        
        fprintf(out, "    %-12s%s <- %-10s;; Loading 3 in %s to indicate syscall to print double value\n",
                "iLoad:", v, "3", v);
        fprintf(out, "    %-12sf12 <- %-10s;; Moving the value to be printed into register f12\n",
                "load.d:", buf);
        fprintf(out, "    write                       ;; This is where syscall will be made\n");
    } else if (is_str) {
        fprintf(out, "    %-12s%s <- %-10s;; Loading 4 in %s to indicate syscall to print string value\n",
                "iLoad:",v , "4", v);
        fprintf(out, "    %-12s%s <- %-10s;; Moving the value to be printed into register %s\n",
                "load:",a, buf, a);
        fprintf(out, "    write                       ;; This is where syscall will be made\n");
    } else {
        fprintf(out, "    %-12s%s <- %-10s;; Loading 1 in %s to indicate syscall to print integer value\n",
                "iLoad:", v, "1", v);
        fprintf(out, "    %-12s%s <- %-10s;; Moving the value to be printed into register %s\n",
                "load:", a, buf,a);
        fprintf(out, "    write                       ;; This is where syscall will be made\n");
    }
} else if (strncmp(lbl, "Num:", 4) == 0) {
        char buf[128]; rtl_num(lbl, buf);
        char *v = rtl_alloc_v();
        char *a = rtl_alloc_a();
        fprintf(out, "    %-12s%s <- %-10s;; Loading 1 in %s to indicate syscall to print integer value\n",
                "iLoad:", v, "1", v);
        fprintf(out, "    %-12s%s <- %-10s;; Moving the value to be printed into register %s\n",
                "iLoad:", a, buf, a);
        fprintf(out, "    write                       ;; This is where syscall will be made\n");
        rtl_free_reg(v); rtl_free_reg(a);

    } else if (expr && !is_leaf(expr)) {
        const char *r = rtl_gen_expr(expr, out);
        char *v = rtl_alloc_v();
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

        /* reset all registers at start of each statement */
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
        char *v = rtl_alloc_v();
        fprintf(out, "    %-12s%s <- %-10s;; String = %s\n",
                "load_addr:", v, sname, buf);
        fprintf(out, "    %-12s%s <- %s\n", "store:", dest, v);
        rtl_free_reg(v);
    } else if (is_leaf(rhs)) {
    if (dest_is_float) {
        rtl_load_into(rhs, "f2", out);   /* loads into f2 directly */
        fprintf(out, "    %-12s%s <- f2\n", "store.d:", dest);
    } else {
        char *v = rtl_alloc_v();
        rtl_load_into(rhs, v, out);
        fprintf(out, "    %-12s%s <- %s\n", "store:", dest, v);
        rtl_free_reg(v);
    }
} else {
        const char *r = rtl_gen_expr(rhs, out);
        if (dest_is_float) {
            fprintf(out, "    %-12s%s <- f2\n", "store.d:", dest);
        } else {
            fprintf(out, "    %-12s%s <- %s\n", "store:", dest, r);
        }
        rtl_free_reg(r);
    }
}

        /* read */
        else if (strncmp(lbl, "Read:", 5) == 0) {
            const char *p = strstr(lbl, ": Name : ");
            if (p) p += 9; else p = lbl + 6;
            char tmp[64]; strcpy(tmp, p);
            char *cut = strchr(tmp, '<'); if (cut) *cut = '\0';
            char *v = rtl_alloc_v();
            fprintf(out, "    %-12s%s <- %-10s;; Loading 5 in %s to indicate syscall to read integer value\n",
                    "iLoad:", v, "5", v);
            fprintf(out, "    read                        ;; This is where syscall will be made\n");
            fprintf(out, "    %-12s%s <- %s          ;; Moving the read value from register %s\n",
                    "store:", tmp, v, v);
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

            char *not_r = rtl_alloc_t();
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

            char *not_r = rtl_alloc_t();
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

        node = node->next;
    }
}

/* ════════════════════════════════════════════
   ENTRY POINT
   ════════════════════════════════════════════ */
inline void print_rtl(FILE *out)
{
    if (!root) return;
    rtl_label_count = 0;
    rtl_str_count   = 0;
    rtl_str_table.clear();
    rtl_reset_regs();

    fprintf(out, "**PROCEDURE: main\n");
    fprintf(out, "**BEGIN: RTL Statements\n");
    rtl_gen_stmt(root, out);
    fprintf(out, "**END: RTL Statements\n");
}

#endif