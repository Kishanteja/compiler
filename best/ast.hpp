#ifndef AST_HPP
#define AST_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ASTNode {
    char label[128];
    struct ASTNode *child1;
    struct ASTNode *child2;
    struct ASTNode *child3;
    struct ASTNode *next;
} ASTNode;

extern ASTNode *root;   /* defined in parser.y */

inline ASTNode *create_node(const char *label)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    strcpy(node->label, label);
    node->child1 = node->child2 = node->child3 = node->next = NULL;
    return node;
}

inline ASTNode *connect_nodes(ASTNode *first, ASTNode *second)
{
    if (!first) return second;
    ASTNode *t = first;
    while (t->next) t = t->next;
    t->next = second;
    return first;
}

inline int is_leaf(ASTNode *node)
{
    return node && !node->child1 && !node->child2 && !node->child3;
}

inline const char *node_type(ASTNode *node)
{
    if (!node) return "int";
    if (strcmp(node->label, "Condition: ?:") == 0)
        return node_type(node->child2);
    char *lt = strrchr(node->label, '<');
    char *gt = strrchr(node->label, '>');
    if (lt && gt && gt > lt) {
        if (strncmp(lt+1, "float",  5) == 0) return "float";
        if (strncmp(lt+1, "bool",   4) == 0) return "bool";
        if (strncmp(lt+1, "string", 6) == 0) return "string";
    }
    return "int";
}

inline const char *arith_type(ASTNode *a, ASTNode *b)
{
    const char *ta = node_type(a);
    const char *tb = node_type(b);
    if (strcmp(ta,"string")==0 || strcmp(tb,"string")==0 ||
        strcmp(ta,"bool")  ==0 || strcmp(tb,"bool")  ==0) {
        fprintf(stderr, "Type error: arithmetic on non-numeric type\n");
        exit(1);
    }
    return (strcmp(ta,"float")==0 || strcmp(tb,"float")==0) ? "float" : "int";
}

static void print_indent(FILE *out, int indent)
{
    for (int i = 0; i < indent; i++) fprintf(out, "  ");
}

inline int print_ast_recursive(ASTNode *node, FILE *out, int indent)
{
    int pending = 0;
    while (node)
    {
        if (pending) { fprintf(out, "\n"); pending = 0; }

        if (strcmp(node->label, "Asgn:") == 0)
        {
            print_indent(out, indent);
            fprintf(out, "Asgn:\n");
            if (node->child1) {
                print_indent(out, indent+4);
                fprintf(out, "LHS (%s)\n", node->child1->label);
            }
            if (node->child2) {
                print_indent(out, indent+4);
                if (is_leaf(node->child2)) {
                    if(node->next){
                        fprintf(out, "RHS (%s)\n", node->child2->label);
                    }
                    else {
                        fprintf(out, "RHS (%s)", node->child2->label);
                        pending = 1;
                    }
                } else {
                    int tc = (strcmp(node->child2->label, "Condition: ?:")==0
                              && node->child2->child1
                              && is_leaf(node->child2->child1));
                    fprintf(out, tc ? "RHS (" : "RHS (\n");
                    int r = print_ast_recursive(node->child2, out, indent+8);
                    if (r) { fprintf(out, ")"); pending = 1; }
                    else   { fprintf(out, ")"); pending = 1; }
                }
            }
        }
        else if (strcmp(node->label, "Condition: ?:") == 0)
        {
            if (node->child1) {
                if (is_leaf(node->child1)) {
                    fprintf(out, "%s", node->child1->label);
                    pending = 1;
                } else {
                    int r = print_ast_recursive(node->child1, out, indent);
                    pending = r;
                }
            }
            if (node->child2) {
                if (pending) { fprintf(out, "\n"); pending = 0; }
                print_indent(out, indent);
                if (is_leaf(node->child2)) {
                    fprintf(out, "True_Part (%s)\n", node->child2->label);
                } else {
                    fprintf(out, "True_Part (\n");
                    int r = print_ast_recursive(node->child2, out, indent+4);
                    if (r) { fprintf(out, ")"); pending = 1; }
                    else   { fprintf(out, ")"); pending = 1; }
                }
            }
            if (node->child3) {
                if (pending) { fprintf(out, "\n"); pending = 0; }
                print_indent(out, indent);
                if (is_leaf(node->child3)) {
                    fprintf(out, "False_Part (%s)", node->child3->label);
                    pending = 1;
                } else {
                    int tc = (strcmp(node->child3->label, "Condition: ?:")==0
                              && node->child3->child1
                              && is_leaf(node->child3->child1));
                    fprintf(out, tc ? "False_Part (" : "False_Part (\n");
                    int r = print_ast_recursive(node->child3, out, indent+4);
                    if (r) { fprintf(out, ")"); pending = 1; }
                    else   { fprintf(out, ")"); pending = 1; }
                }
            }
        }
        /* ── if / if-else ── */
else if (strcmp(node->label, "If:") == 0)
{
    print_indent(out, indent);
    fprintf(out, "If:\n");

    /* Condition */
    if (node->child1) {
        print_indent(out, indent+4);
        if (is_leaf(node->child1)) {
            fprintf(out, "Condition (%s)\n", node->child1->label);
        } else {
            fprintf(out, "Condition (\n");
            int r = print_ast_recursive(node->child1, out, indent+8);
            if (r) fprintf(out, ")\n"); else fprintf(out, ")\n");
        }
    }

    /* Then */
if (node->child2) {
    print_indent(out, indent+4);
    fprintf(out, "Then (\n");
    int r = print_ast_recursive(node->child2, out, indent+8);
    if (!node->child3) {
        /* Then is last — no \n */
        if (r) { fprintf(out, ")"); pending = 1; }
        else   { fprintf(out, ")"); pending = 1; }
    } else {
        if (r) fprintf(out, ")\n"); else fprintf(out, ")\n");
    }
}

/* Else */
if (node->child3) {
    print_indent(out, indent+4);
    fprintf(out, "Else (\n");
    int r = print_ast_recursive(node->child3, out, indent+8);
    /* Else is always last — no \n */
    if (r) { fprintf(out, ")"); pending = 1; }
    else   { fprintf(out, ")"); pending = 1; }
}
}

/* ── while ── */
else if (strcmp(node->label, "While:") == 0)
{
    print_indent(out, indent);
    fprintf(out, "While:\n");

    if (node->child1) {
        print_indent(out, indent+4);
        if (is_leaf(node->child1)) {
            fprintf(out, "Condition (%s)\n", node->child1->label);
        } else {
            fprintf(out, "Condition (\n");
            int r = print_ast_recursive(node->child1, out, indent+8);
            if (r) fprintf(out, ")\n"); else fprintf(out, ")\n");
        }
    }
    if (node->child2) {
    print_indent(out, indent+4);
    fprintf(out, "Body (\n");
    int r = print_ast_recursive(node->child2, out, indent+8);
    /* Body is last — no \n */
    if (r) { fprintf(out, ")"); pending = 1; }
    else   { fprintf(out, ")"); pending = 1; }
}
}

/* ── do-while ── */
else if (strcmp(node->label, "Do:") == 0)
{
    print_indent(out, indent);
    fprintf(out, "Do:\n");

    /* Body first */
    if (node->child1) {
        print_indent(out, indent+4);
        fprintf(out, "Body (\n");
        int r = print_ast_recursive(node->child1, out, indent+8);
        if (r) fprintf(out, ")\n"); else fprintf(out, ")\n");
    }

    /* While Condition after */
    if (node->child2) {
    print_indent(out, indent+4);
    if (is_leaf(node->child2)) {
        fprintf(out, "While Condition (%s)", node->child2->label);
        pending = 1;
    } else {
        fprintf(out, "While Condition (\n");
        int r = print_ast_recursive(node->child2, out, indent+8);
        /* always last — no \n */
        if (r) { fprintf(out, ")"); pending = 1; }
        else   { fprintf(out, ")"); pending = 1; }
    }
}
}
        else if (node->child1 && !node->child2)
        {
            print_indent(out, indent);
            fprintf(out, "%s\n", node->label);
            print_indent(out, indent+4);
            if (is_leaf(node->child1)) {
                fprintf(out, "Opd (%s)", node->child1->label);
                pending = 1;
            } else {
                fprintf(out, "Opd (\n");
                int r = print_ast_recursive(node->child1, out, indent+8);
                if (r) { fprintf(out, ")"); pending = 1; }
                else   { fprintf(out, ")"); pending = 1; }
            }
        }
        else if (node->child1 && node->child2)
        {
            print_indent(out, indent);
            fprintf(out, "%s\n", node->label);
            print_indent(out, indent+4);
            if (is_leaf(node->child1)) {
                fprintf(out, "L_Opd (%s)\n", node->child1->label);
            } else {
                fprintf(out, "L_Opd (\n");
                int r = print_ast_recursive(node->child1, out, indent+8);
                if (r) fprintf(out, ")\n"); else fprintf(out, ")\n");
            }
            print_indent(out, indent+4);
            if (is_leaf(node->child2)) {
                fprintf(out, "R_Opd (%s)", node->child2->label);
                pending = 1;
            } else {
                fprintf(out, "R_Opd (\n");
                int r = print_ast_recursive(node->child2, out, indent+8);
                if (r) { fprintf(out, ")"); pending = 1; }
                else   { fprintf(out, ")"); pending = 1; }
            }
        }
        else if (strncmp(node->label, "Read:", 5) == 0)
        {
            print_indent(out, indent);
            fprintf(out, "%s", node->label);
            pending = 1;
        }
        else if (strncmp(node->label, "Write:", 6) == 0)
{
    print_indent(out, indent);
    if (!node->child1) {
        fprintf(out, "%s", node->label);   /* no \n */
        pending = 1;
    } else {
        fprintf(out, "%s\n", node->label);
        /* child1 handling stays exactly the same */
        print_indent(out, indent+4);
        if (is_leaf(node->child1)) {
            fprintf(out, "Opd (%s)", node->child1->label);
            pending = 1;
        } else {
            fprintf(out, "Opd (\n");
            int r = print_ast_recursive(node->child1, out, indent+8);
            if (r) { fprintf(out, ")"); pending = 1; }
            else   { fprintf(out, ")"); pending = 1; }
        }
    }
}
        else
        {
            print_indent(out, indent);
            if (strncmp(node->label, "Name:", 5) == 0) {
                fprintf(out, "%s", node->label);
                pending = 1;
            } else {
                fprintf(out, "%s\n", node->label);
                pending = 0;
            }
        }
        node = node->next;
    }
    return pending;
}

inline void print_ast(FILE *out)
{
    if (!root) return;
    fprintf(out, "**PROCEDURE: main\n");
    fprintf(out, "\tReturn Type: <void>\n");
    fprintf(out, "\tFormal Parameters:\n");
    fprintf(out, "**BEGIN: Abstract Syntax Tree\n");
    int r = print_ast_recursive(root, out, 9);
    if (r) fprintf(out, "\n");
    fprintf(out, "**END: Abstract Syntax Tree\n");
}

#endif