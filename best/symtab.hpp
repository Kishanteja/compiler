#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMS 256

typedef struct {
    char name[64];
    char type[16];
} Symbol;

extern Symbol sym_table[MAX_SYMS];
extern int    sym_count;
extern Symbol local_table[MAX_SYMS];
extern int    local_count;
extern int    in_function;
extern const char *current_decl_type;

inline void sym_insert(const char *name, const char *type)
{
    Symbol *table = in_function ? local_table : sym_table;
    int    *count = in_function ? &local_count : &sym_count;
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

#endif