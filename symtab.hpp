#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMS   256
#define MAX_FUNCS  256
#define MAX_PARAMS  32

/* ════════════════════════════════════════════
   VARIABLE SYMBOL TABLE
   ════════════════════════════════════════════ */
typedef struct {
    char name[64];
    char type[16];
} Symbol;

extern Symbol      sym_table[MAX_SYMS];
extern int         sym_count;
extern Symbol      local_table[MAX_SYMS];
extern int         local_count;
extern int         in_function;
extern const char *current_decl_type;

inline void sym_insert(const char *name, const char *type)
{
    Symbol *table = in_function ? local_table : sym_table;
    int    *count = in_function ? &local_count : &sym_count;
    if (strcmp(name, "main") == 0) {
    fprintf(stderr, "Error: 'main' cannot be used as a variable name\n");
    exit(1);
}
    for (int i = 0; i < *count; i++) {
        if (strcmp(table[i].name, name) == 0) {
            fprintf(stderr, "Error: redeclaration of '%s' in same scope\n", name);
            exit(1);
        }
    }
    strcpy(table[*count].name, name);
    strcpy(table[*count].type, type);
    (*count)++;
}

inline const char *sym_lookup(const char *name)
{
    for (int i = 0; i < local_count; i++)
        if (strcmp(local_table[i].name, name) == 0)
            return local_table[i].type;
    for (int i = 0; i < sym_count; i++)
        if (strcmp(sym_table[i].name, name) == 0)
            return sym_table[i].type;
    fprintf(stderr, "Error: undeclared variable '%s'\n", name);
    exit(1);
}

/* ════════════════════════════════════════════
   FUNCTION SYMBOL TABLE
   ════════════════════════════════════════════
   Each FuncSymbol stores:
     - body      : AST root for this function's body
     - defined   : 1 if definition seen, 0 if only declared
     - local_syms: snapshot of locals (for codegen)
   Codegen iterates func_table to emit code for every function.
*/

struct ASTNode;   /* forward declaration */

typedef struct {
    char name[64];
    char ret_type[16];
    int  param_count;
    char param_names[MAX_PARAMS][64];
    char param_types[MAX_PARAMS][16];

    /* ── per-function storage ── */
    struct ASTNode *body;
    int   defined;
    Symbol local_syms[MAX_SYMS];
    int    local_sym_count;
    int    def_order;              /* set when func_set_body is called */
} FuncSymbol;

extern FuncSymbol func_table[MAX_FUNCS];
extern int        func_count;
extern const char *current_function;

/* Insert a new function (or silently skip if already present). */
inline void func_insert(const char *name, const char *ret_type)
{
    for (int i = 0; i < func_count; i++)
        if (strcmp(func_table[i].name, name) == 0) return;
    strcpy(func_table[func_count].name,     name);
    strcpy(func_table[func_count].ret_type, ret_type);
    func_table[func_count].param_count     = 0;
    func_table[func_count].body            = NULL;
    func_table[func_count].defined         = 0;
    func_table[func_count].local_sym_count = 0;
    func_table[func_count].def_order = -1;
    func_count++;
}

/* Attach a body AST and snapshot the current locals. */
inline void func_set_body(const char *name, struct ASTNode *body_ast)
{
    for (int i = 0; i < func_count; i++) {
        if (strcmp(func_table[i].name, name) == 0) {
            if (func_table[i].defined) {
                fprintf(stderr, "Error: redefinition of function '%s'\n", name);
                exit(1);
            }
            func_table[i].body    = body_ast;
            func_table[i].defined = 1;
            /* snapshot locals so codegen can restore them later */
            func_table[i].local_sym_count = local_count;
            memcpy(func_table[i].local_syms, local_table,
                   local_count * sizeof(Symbol));
            static int func_def_counter = 0;   // put at top of file or before func_set_body
            func_table[i].def_order = func_def_counter++;
            return;
        }
    }
}

inline void func_reset_params(const char *name)
{
    for (int i = 0; i < func_count; i++)
        if (strcmp(func_table[i].name, name) == 0)
            return;
}

inline void func_add_param(const char *fname, const char *pname, const char *ptype)
{
    for (int i = 0; i < func_count; i++) {
        if (strcmp(func_table[i].name, fname) == 0) {
            int pc = func_table[i].param_count;
            strcpy(func_table[i].param_names[pc], pname);
            strcpy(func_table[i].param_types[pc], ptype);
            func_table[i].param_count++;
            return;
        }
    }
}

inline const char *func_lookup(const char *name)
{
    for (int i = 0; i < func_count; i++)
        if (strcmp(func_table[i].name, name) == 0)
            return func_table[i].ret_type;
    return NULL;
}

inline int func_param_count(const char *name)
{
    for (int i = 0; i < func_count; i++)
        if (strcmp(func_table[i].name, name) == 0)
            return func_table[i].param_count;
    return -1;
}

inline const char *func_param_type(const char *name, int idx)
{
    for (int i = 0; i < func_count; i++)
        if (strcmp(func_table[i].name, name) == 0) {
            if (idx < func_table[i].param_count)
                return func_table[i].param_types[idx];
            return NULL;
        }
    return NULL;
}

/* Helper: get FuncSymbol* by index (for codegen loops) */
inline FuncSymbol *func_get(int idx)
{
    if (idx >= 0 && idx < func_count) return &func_table[idx];
    return NULL;
}

/* Helper: find index by name */
inline int func_index(const char *name)
{
    for (int i = 0; i < func_count; i++)
        if (strcmp(func_table[i].name, name) == 0) return i;
    return -1;
}

inline void func_update_param_name(const char *fname, int idx, const char *new_name)
{
    for (int i = 0; i < func_count; i++) {
        if (strcmp(func_table[i].name, fname) == 0) {
            if (idx < func_table[i].param_count)
                strcpy(func_table[i].param_names[idx], new_name);
            return;
        }
    }
}

#endif