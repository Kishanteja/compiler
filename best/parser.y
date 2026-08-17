/* ════════════════════════════════════════════════════════════
   parser.y  —  Grammar Rules + Type Checks
   ════════════════════════════════════════════════════════════ */

%{
#include "symtab.hpp"
#include "tac.hpp"

/* ── global definitions (one translation unit only) ── */
ASTNode    *root            = NULL;
Symbol      sym_table[MAX_SYMS];
int         sym_count       = 0;
Symbol      local_table[MAX_SYMS];
int         local_count     = 0;
int         in_function     = 0;
const char *current_decl_type = "int";

int yylex(void);
int yyerror(const char *s);
%}

%code requires {
#include "ast.hpp"
#include "symtab.hpp"
}

%union { ASTNode *node; char *str; }

%token INTEGER FLOAT STRING VOID BOOL
%token <str> NAME INT_NUM FLOAT_NUM STR_CONST BOOL_CONST
%token READ WRITE
%token ASSIGN_OP
%token PLUS MINUS MUL DIV
%token OR AND NOT
%token EQUAL NOT_EQUAL
%token GREATER GREATER_EQUAL LESS LESS_EQUAL
%token QUESTION_MARK COLON
%token SEMICOLON COMMA
%token LEFT_ROUND_BRACKET  RIGHT_ROUND_BRACKET
%token LEFT_CURLY_BRACKET  RIGHT_CURLY_BRACKET
%token IF ELSE WHILE DO

/* precedence: lowest → highest */
%right QUESTION_MARK COLON
%left  OR
%left  AND
%left  EQUAL NOT_EQUAL
%left  GREATER GREATER_EQUAL LESS LESS_EQUAL
%left  PLUS MINUS
%left  MUL DIV
%right NOT
%right UMINUS

%type <node> program function_list function_item function_def function_header
%type <node> compound_statement block_items block_item
%type <node> statement assignment_statement read_statement print_statement
%type <node> expression conditional_expression
%type <node> logical_or_expression logical_and_expression
%type <node> not_expression equality_expression
%type <node> relational_expression additive_expression term factor
%type <node> if_statement while_statement do_while_statement

%start program

%%

program
    : global_decl_list function_list  { root = $2; }
    ;

function_list
    : function_list function_item  { $$ = connect_nodes($1, $2); }
    | function_item                { $$ = $1; }
    ;

function_item
    : function_decl  { $$ = NULL; }
    | function_def   { $$ = $1;   }
    ;

function_decl
    : VOID NAME LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON
    ;

function_header
    : VOID NAME LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET
      { in_function = 1; local_count = 0; }
    ;

function_def
    : function_header compound_statement
      { $$ = $2; in_function = 0; }
    | function_header compound_statement SEMICOLON
      { $$ = $2; in_function = 0; }
    ;

global_decl_list
    : global_decl_list var_decl_stmt
    | /* empty */
    ;

var_decl_stmt
    : named_type var_decl_item_list SEMICOLON
    ;

var_decl_item_list
    : var_decl_item_list COMMA NAME
      { sym_insert($3, current_decl_type); free($3); }
    | NAME
      { sym_insert($1, current_decl_type); free($1); }
    ;

named_type
    : INTEGER  { current_decl_type = "int";    }
    | FLOAT    { current_decl_type = "float";  }
    | STRING   { current_decl_type = "string"; }
    | BOOL     { current_decl_type = "bool";   }
    | VOID     { current_decl_type = "void";   }
    ;

compound_statement
    : LEFT_CURLY_BRACKET block_items RIGHT_CURLY_BRACKET  { $$ = $2; }
    ;

block_items
    : block_items block_item  { $$ = connect_nodes($1, $2); }
    | /* empty */             { $$ = NULL; }
    ;

block_item
    : var_decl_stmt  { $$ = NULL; }
    | statement      { $$ = $1;   }
    ;

statement
    : assignment_statement  { $$ = $1; }
    | read_statement        { $$ = $1; }
    | print_statement       { $$ = $1; }
    | if_statement          { $$ = $1; }
    | while_statement       { $$ = $1; }
    | do_while_statement    { $$ = $1; }
    | compound_statement    { $$ = $1; } 
    ;

read_statement
    : READ NAME SEMICOLON
      {
          const char *t = sym_lookup($2);
          if (strcmp(t, "int") != 0 && strcmp(t, "float") != 0) {
              fprintf(stderr, "Type error: read only valid for int/float\n");
              exit(1);
          }
          char buf[256];
          sprintf(buf, "Read: Name : %s_<%s>", $2, t);
          free($2);
          $$ = create_node(buf);
      }
    ;

print_statement
    : WRITE expression SEMICOLON
      {
          if ($2 && !$2->child1 && !$2->child2 && !$2->child3) {
              char buf[256];
              sprintf(buf, "Write: %s", $2->label);
              $$ = create_node(buf);
          } else {
              ASTNode *n = create_node("Write:");
              n->child1 = $2;
              $$ = n;
          }
      }
    ;

assignment_statement
    : NAME ASSIGN_OP expression SEMICOLON
      {
          const char *lhs_type = sym_lookup($1);
          const char *rhs_type = node_type($3);
          if (strcmp(lhs_type, rhs_type) != 0) {
              fprintf(stderr, "Type error: cannot assign <%s> to <%s> variable '%s'\n",
                      rhs_type, lhs_type, $1);
              exit(1);
          }
          ASTNode *assign = create_node("Asgn:");
          char buf[256];
          sprintf(buf, "Name: %s_<%s>", $1, lhs_type);
          free($1);
          assign->child1 = create_node(buf);
          assign->child2 = $3;
          $$ = assign;
      }
    ;

if_statement
    : IF LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement
      {
          if (strcmp(node_type($3), "bool") != 0) {
              fprintf(stderr, "Type error: if condition must be bool\n");
              exit(1);
          }
          ASTNode *n = create_node("If:");
          n->child1 = $3;   /* condition */
          n->child2 = $5;   /* then      */
          n->child3 = NULL; /* no else   */
          $$ = n;
      }
    | IF LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement ELSE statement
      {
          if (strcmp(node_type($3), "bool") != 0) {
              fprintf(stderr, "Type error: if condition must be bool\n");
              exit(1);
          }
          ASTNode *n = create_node("If:");
          n->child1 = $3;   /* condition */
          n->child2 = $5;   /* then      */
          n->child3 = $7;   /* else      */
          $$ = n;
      }
    ;

while_statement
    : WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement
      {
          if (strcmp(node_type($3), "bool") != 0) {
              fprintf(stderr, "Type error: while condition must be bool\n");
              exit(1);
          }
          ASTNode *n = create_node("While:");
          n->child1 = $3;   /* condition */
          n->child2 = $5;   /* body      */
          $$ = n;
      }
    ;

do_while_statement
    : DO statement WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET SEMICOLON
      {
          if (strcmp(node_type($5), "bool") != 0) {
              fprintf(stderr, "Type error: do-while condition must be bool\n");
              exit(1);
          }
          ASTNode *n = create_node("Do:");
          n->child1 = $2;   /* body      */
          n->child2 = $5;   /* condition */
          $$ = n;
      }
    ;

expression
    : conditional_expression  { $$ = $1; }
    ;

conditional_expression
    : logical_or_expression  { $$ = $1; }
    | logical_or_expression QUESTION_MARK expression COLON conditional_expression
      {
          if (strcmp(node_type($1), "bool") != 0) {
              fprintf(stderr, "Type error: ternary condition must be bool\n");
              exit(1);
          }
          if (strcmp(node_type($3), node_type($5)) != 0) {
              fprintf(stderr, "Type error: ternary branches must have same type\n");
              exit(1);
          }
          ASTNode *n = create_node("Condition: ?:");
          n->child1 = $1; n->child2 = $3; n->child3 = $5;
          $$ = n;
      }
    ;

logical_or_expression
    : logical_or_expression OR logical_and_expression
      {
          if (strcmp(node_type($1),"bool")!=0 || strcmp(node_type($3),"bool")!=0) {
              fprintf(stderr, "Type error: || requires bool operands\n");
              exit(1);
          }
          ASTNode *n = create_node("Condition: OR<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | logical_and_expression  { $$ = $1; }
    ;

logical_and_expression
    : logical_and_expression AND not_expression
      {
          if (strcmp(node_type($1),"bool")!=0 || strcmp(node_type($3),"bool")!=0) {
              fprintf(stderr, "Type error: && requires bool operands\n");
              exit(1);
          }
          ASTNode *n = create_node("Condition: AND<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | not_expression  { $$ = $1; }
    ;

not_expression
    : NOT not_expression
      {
          if (strcmp(node_type($2), "bool") != 0) {
              fprintf(stderr, "Type error: ! requires bool operand\n");
              exit(1);
          }
          ASTNode *n = create_node("Condition: NOT<bool>");
          n->child1 = $2; $$ = n;
      }
    | equality_expression  { $$ = $1; }
    ;

equality_expression
    : equality_expression EQUAL relational_expression
      {
          if (strcmp(node_type($1), node_type($3)) != 0) {
              fprintf(stderr, "Type error: cannot compare <%s> with <%s>\n",
                      node_type($1), node_type($3));
              exit(1);
          }
          ASTNode *n = create_node("Condition: EQ<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | equality_expression NOT_EQUAL relational_expression
      {
          if (strcmp(node_type($1), node_type($3)) != 0) {
              fprintf(stderr, "Type error: cannot compare <%s> with <%s>\n",
                      node_type($1), node_type($3));
              exit(1);
          }
          ASTNode *n = create_node("Condition: NE<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | relational_expression  { $$ = $1; }
    ;

relational_expression
    : relational_expression LESS additive_expression
      {
          if (strcmp(node_type($1),"string")==0||strcmp(node_type($1),"bool")==0||
              strcmp(node_type($3),"string")==0||strcmp(node_type($3),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n");
              exit(1);
          }
          ASTNode *n = create_node("Condition: LT<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | relational_expression GREATER additive_expression
      {
          if (strcmp(node_type($1),"string")==0||strcmp(node_type($1),"bool")==0||
              strcmp(node_type($3),"string")==0||strcmp(node_type($3),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n");
              exit(1);
          }
          ASTNode *n = create_node("Condition: GT<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | relational_expression GREATER_EQUAL additive_expression
      {
          if (strcmp(node_type($1),"string")==0||strcmp(node_type($1),"bool")==0||
              strcmp(node_type($3),"string")==0||strcmp(node_type($3),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n");
              exit(1);
          }
          ASTNode *n = create_node("Condition: GE<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | relational_expression LESS_EQUAL additive_expression
      {
          if (strcmp(node_type($1),"string")==0||strcmp(node_type($1),"bool")==0||
              strcmp(node_type($3),"string")==0||strcmp(node_type($3),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n");
              exit(1);
          }
          ASTNode *n = create_node("Condition: LE<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | additive_expression  { $$ = $1; }
    ;

additive_expression
    : additive_expression PLUS term
      {
          char buf[64];
          sprintf(buf, "Arith: Plus<%s>", arith_type($1,$3));
          ASTNode *n = create_node(buf);
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | additive_expression MINUS term
      {
          char buf[64];
          sprintf(buf, "Arith: Minus<%s>", arith_type($1,$3));
          ASTNode *n = create_node(buf);
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | term  { $$ = $1; }
    ;

term
    : term MUL factor
      {
          char buf[64];
          sprintf(buf, "Arith: Mult<%s>", arith_type($1,$3));
          ASTNode *n = create_node(buf);
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | term DIV factor
      {
          char buf[64];
          sprintf(buf, "Arith: Div<%s>", arith_type($1,$3));
          ASTNode *n = create_node(buf);
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | factor  { $$ = $1; }
    ;

factor
    : NAME
      {
          char buf[256];
          sprintf(buf, "Name: %s_<%s>", $1, sym_lookup($1));
          free($1); $$ = create_node(buf);
      }
    | INT_NUM
      {
          char buf[128];
          sprintf(buf, "Num: %s<int>", $1);
          $$ = create_node(buf); free($1);
      }
    | FLOAT_NUM
      {
          char buf[128];
          sprintf(buf, "Num: %.2f<float>", atof($1));
          $$ = create_node(buf); free($1);
      }
    | STR_CONST
      {
          char buf[128];
          sprintf(buf, "Str: %s<string>", $1);
          $$ = create_node(buf); free($1);
      }
    | BOOL_CONST
      {
          char buf[128];
          sprintf(buf, "Bool: %s<bool>", $1);
          $$ = create_node(buf); free($1);
      }
    | LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET
      { $$ = $2; }
    | MINUS factor
      {
          if (strcmp(node_type($2),"string")==0||strcmp(node_type($2),"bool")==0) {
              fprintf(stderr, "Type error: unary minus on non-numeric type\n");
              exit(1);
          }
          char buf[64];
          sprintf(buf, "Arith: Uminus<%s>", node_type($2));
          ASTNode *n = create_node(buf);
          n->child1 = $2; $$ = n;
      }
    ;

%%

int yyerror(const char *s)
{
    fprintf(stderr, "Parser error\n");
    exit(1);
}