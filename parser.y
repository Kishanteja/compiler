/* ════════════════════════════════════════════════════════════
   parser.y  —  Grammar Rules + Type Checks
   UPGRADED: multi-function support following reference grammar
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

/* ── function table globals ── */
FuncSymbol  func_table[MAX_FUNCS];
int         func_count      = 0;
const char *current_function = NULL;
static char saved_func_name[64];
static int  params_already_set = 0;
static int  param_check_idx    = 0;
static int  has_return_stmt    = 0;

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
%token READ WRITE RETURN
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

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%type <node> program
%type <node> func_def_list func_def func_header
%type <node> compound_statement block_items block_stmts stmt_block
%type <node> statement assignment_statement read_statement print_statement
%type <node> call_statement func_call
%type <node> return_statement
%type <node> expression conditional_expression
%type <node> logical_or_expression logical_and_expression
%type <node> not_expression equality_expression
%type <node> relational_expression additive_expression term factor
%type <node> if_statement while_statement do_while_statement
%type <node> actual_arg_list non_empty_arg_list

%start program

%%

/* ════════════════════════════════════════════════════════════
   TOP LEVEL — follows reference grammar structure
   ──────────────────────────────────────────────────────────
   program = optional declarations … then function definitions

   global_decl_statement_list contains var_decl_stmt and func_decl
   func_def_list contains func_def

   Both func_decl and func_def share func_header '(' … ')'.
   After ')' the lookahead ';' vs '{' resolves which one.
   No mid-rule actions needed → zero conflicts.
   ════════════════════════════════════════════════════════════ */
program
    : global_decl_statement_list func_def_list
      {
          int mi = func_index("main");
          root = (mi >= 0) ? func_table[mi].body : NULL;
          $$ = $2;
      }
    | func_def_list
      {
          int mi = func_index("main");
          root = (mi >= 0) ? func_table[mi].body : NULL;
          $$ = $1;
      }
    ;

/* ── declarations (global vars + forward-declared functions) ── */
global_decl_statement_list
    : global_decl_statement_list func_decl
    | global_decl_statement_list var_decl_stmt
    | var_decl_stmt
    | func_decl
    ;

/* ── function definitions (one or more) ── */
func_def_list
    : func_def_list func_def  { $$ = NULL; }
    | func_def                { $$ = NULL; }
    ;

/* ════════════════════════════════════════════
   FUNC_HEADER  — the key non-terminal
   Reduces  named_type NAME  into one unit
   BEFORE '(' is seen.  No mid-rule actions.
   ════════════════════════════════════════════ */
func_header
    : named_type NAME
      {
          params_already_set = (func_lookup($2) != NULL);
          func_insert($2, current_decl_type);
          strcpy(saved_func_name, $2);
          current_function = saved_func_name;
          in_function  = 1;
          local_count  = 0;
          param_check_idx = 0;
          has_return_stmt = 0;
          free($2);
          $$ = NULL;
      }
    ;

/* ════════════════════════════════════════════
   FUNCTION DECLARATION (forward prototype)
   func_header '(' params? ')' ';'
   ════════════════════════════════════════════ */
func_decl
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET SEMICOLON
      {
          in_function      = 0;
          current_function = NULL;
          local_count      = 0;
      }
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON
      {
          in_function      = 0;
          current_function = NULL;
          local_count      = 0;
      }
    ;

/* ════════════════════════════════════════════
   FUNCTION DEFINITION
   func_header '(' params? ')' compound_statement
   ════════════════════════════════════════════ */
func_def
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET compound_statement
      {
          const char *ret = func_lookup(saved_func_name);
          if (ret && strcmp(ret, "void") != 0 && !has_return_stmt) {
              fprintf(stderr,
                  "Error: non-void function '%s' must have a return statement\n",
                  saved_func_name);
              exit(1);
          }
          func_set_body(saved_func_name, $5);
          in_function      = 0;
          current_function = NULL;
          $$ = $5;
      }
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET compound_statement
      {
          const char *ret = func_lookup(saved_func_name);
          if (ret && strcmp(ret, "void") != 0 && !has_return_stmt) {
              fprintf(stderr,
                  "Error: non-void function '%s' must have a return statement\n",
                  saved_func_name);
              exit(1);
          }
          func_set_body(saved_func_name, $4);
          in_function      = 0;
          current_function = NULL;
          $$ = $4;
      }
    ;

/* ════════════════════════════════════════════
   FORMAL PARAMETER LIST
   ════════════════════════════════════════════ */
formal_param_list
    : formal_param_list COMMA formal_param
    | formal_param
    ;

formal_param
    : param_type NAME
      {
          /* always add to local scope (for function body) */
          sym_insert($2, current_decl_type);
          /* if a prior declaration exists, cross-check param types */
          if (params_already_set) {
              const char *decl_type = func_param_type(saved_func_name, param_check_idx);
              if (decl_type && strcmp(decl_type, current_decl_type) != 0) {
                  fprintf(stderr,
                      "Error: parameter %d of '%s' declared as <%s> but defined as <%s>\n",
                      param_check_idx + 1, saved_func_name, decl_type, current_decl_type);
                  exit(1);
              }
              func_update_param_name(saved_func_name, param_check_idx, $2);
          } else {
              func_add_param(saved_func_name, $2, current_decl_type);
          }
          param_check_idx++;
          free($2);
      }
    ;

/* param_type: like named_type but without VOID (no void params) */
param_type
    : INTEGER  { current_decl_type = "int";    }
    | FLOAT    { current_decl_type = "float";  }
    | STRING   { current_decl_type = "string"; }
    | BOOL     { current_decl_type = "bool";   }
    ;

/* ════════════════════════════════════════════
   VARIABLE DECLARATIONS
   ════════════════════════════════════════════ */
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

/* ════════════════════════════════════════════
   COMPOUND STATEMENT / BLOCK
   ════════════════════════════════════════════ */
compound_statement
    : LEFT_CURLY_BRACKET block_items RIGHT_CURLY_BRACKET  { $$ = $2; }
    ;

block_items
    : block_decls block_stmts  { $$ = $2; }
    | block_decls              { $$ = NULL; }
    | block_stmts              { $$ = $1; }
    | /* empty */              { $$ = NULL; }
    ;

block_decls
    : block_decls var_decl_stmt
    | var_decl_stmt
    ;

block_stmts
    : block_stmts statement  { $$ = connect_nodes($1, $2); }
    | statement              { $$ = $1; }
    ;

stmt_block
    : LEFT_CURLY_BRACKET block_stmts RIGHT_CURLY_BRACKET  { $$ = $2; }
    | LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET              { $$ = NULL; }
    ;

/* ════════════════════════════════════════════
   STATEMENTS
   ════════════════════════════════════════════ */
statement
    : assignment_statement  { $$ = $1; }
    | read_statement        { $$ = $1; }
    | print_statement       { $$ = $1; }
    | if_statement          { $$ = $1; }
    | while_statement       { $$ = $1; }
    | do_while_statement    { $$ = $1; }
    | return_statement      { $$ = $1; }
    | call_statement        { $$ = $1; }
    | stmt_block            { $$ = $1; }
    ;

/* ════════════════════════════════════════════
   FUNCTION CALL  (shared non-terminal)
   Used in both call_statement and factor
   ════════════════════════════════════════════ */
func_call
    : NAME LEFT_ROUND_BRACKET actual_arg_list RIGHT_ROUND_BRACKET
      {
          const char *ret = func_lookup($1);
          if (!ret) {
              fprintf(stderr, "Error: call to undeclared function '%s'\n", $1);
              exit(1);
          }
          /* check argument count */
          int expected = func_param_count($1);
          int actual = 0;
          ASTNode *a = $3;
          while (a) { actual++; a = a->next; }
          if (actual != expected) {
              fprintf(stderr,
                  "Error: '%s' expects %d args, got %d\n",
                  $1, expected, actual);
              exit(1);
          }
          /* check argument types */
          a = $3;
          for (int i = 0; i < expected; i++) {
              const char *pt = func_param_type($1, i);
              const char *at = node_type(a);
              if (strcmp(pt, at) != 0) {
                  fprintf(stderr,
                      "Type error: arg %d of '%s' expects <%s>, got <%s>\n",
                      i+1, $1, pt, at);
                  exit(1);
              }
              a = a->next;
          }
          char buf[128];
          sprintf(buf, "FuncCall: %s<%s>", $1, ret);
          ASTNode *n = create_node(buf);
          n->child1 = $3;
          free($1);
          $$ = n;
      }
    ;

actual_arg_list
    : non_empty_arg_list  { $$ = $1;   }
    | /* empty */         { $$ = NULL; }
    ;

non_empty_arg_list
    : non_empty_arg_list COMMA expression  { $$ = connect_nodes($1, $3); }
    | expression                           { $$ = $1; }
    ;

/* ── CALL STATEMENT: standalone function call ── */
call_statement
    : func_call SEMICOLON
      {
          const char *caller_ret = func_lookup(saved_func_name);
          if (caller_ret && strcmp(caller_ret, "void") != 0) {
              char fname[64];
              const char *p = $1->label + 10;
              const char *cut = strchr(p, '<');
              if (cut) { int n=cut-p; strncpy(fname,p,n); fname[n]='\0'; }
              else strcpy(fname, p);
              const char *callee_ret = func_lookup(fname);
              if (callee_ret && strcmp(callee_ret, "void") != 0) {
                  fprintf(stderr,
                      "Error: return value of function '%s' is ignored\n", fname);
                  exit(1);
              }
          }
          $$ = $1;
      }
    ;

/* ── RETURN STATEMENT ── */
return_statement
    : RETURN expression SEMICOLON
      {
          const char *fn_ret  = func_lookup(saved_func_name);
          const char *ex_type = node_type($2);
          if (fn_ret && strcmp(fn_ret, "void") == 0) {
              fprintf(stderr,
                  "Type error: void function '%s' cannot return a value\n",
                  saved_func_name);
              exit(1);
          }
          if (fn_ret && strcmp(fn_ret, ex_type) != 0) {
              fprintf(stderr,
                  "Type error: function '%s' returns <%s>, got <%s>\n",
                  saved_func_name, fn_ret, ex_type);
              exit(1);
          }
          char buf[128];
          sprintf(buf, "Return:<%s>", ex_type);
          ASTNode *n = create_node(buf);
          n->child1 = $2;
          has_return_stmt = 1;
          $$ = n;
      }
    | RETURN SEMICOLON
      {
          const char *fn_ret = func_lookup(saved_func_name);
          if (fn_ret && strcmp(fn_ret, "void") != 0) {
              fprintf(stderr,
                  "Type error: non-void function '%s' must return a value\n",
                  saved_func_name);
              exit(1);
          }
          has_return_stmt = 1;
          $$ = create_node("Return:<void>");
      }
    ;

/* ════════════════════════════════════════════
   READ / WRITE / ASSIGNMENT  (unchanged)
   ════════════════════════════════════════════ */
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

/* ════════════════════════════════════════════
   CONTROL FLOW  (unchanged)
   ════════════════════════════════════════════ */
if_statement
    : IF LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement %prec LOWER_THAN_ELSE
      {
          if (strcmp(node_type($3), "bool") != 0) {
              fprintf(stderr, "Type error: if condition must be bool\n"); exit(1);
          }
          ASTNode *n = create_node("If:");
          n->child1 = $3; n->child2 = $5; n->child3 = NULL;
          $$ = n;
      }
    | IF LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement ELSE statement
      {
          if (strcmp(node_type($3), "bool") != 0) {
              fprintf(stderr, "Type error: if condition must be bool\n"); exit(1);
          }
          ASTNode *n = create_node("If:");
          n->child1 = $3; n->child2 = $5; n->child3 = $7;
          $$ = n;
      }
    ;

while_statement
    : WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement
      {
          if (strcmp(node_type($3), "bool") != 0) {
              fprintf(stderr, "Type error: while condition must be bool\n"); exit(1);
          }
          ASTNode *n = create_node("While:");
          n->child1 = $3; n->child2 = $5;
          $$ = n;
      }
    ;

do_while_statement
    : DO statement WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET SEMICOLON
      {
          if (strcmp(node_type($5), "bool") != 0) {
              fprintf(stderr, "Type error: do-while condition must be bool\n"); exit(1);
          }
          ASTNode *n = create_node("Do:");
          n->child1 = $2; n->child2 = $5;
          $$ = n;
      }
    ;

/* ════════════════════════════════════════════
   EXPRESSIONS  (unchanged except factor)
   ════════════════════════════════════════════ */
expression
    : conditional_expression  { $$ = $1; }
    ;

conditional_expression
    : logical_or_expression  { $$ = $1; }
    | logical_or_expression QUESTION_MARK expression COLON conditional_expression
      {
          if (strcmp(node_type($1), "bool") != 0) {
              fprintf(stderr, "Type error: ternary condition must be bool\n"); exit(1);
          }
          if (strcmp(node_type($3), node_type($5)) != 0) {
              fprintf(stderr, "Type error: ternary branches must have same type\n"); exit(1);
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
              fprintf(stderr, "Type error: || requires bool operands\n"); exit(1);
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
              fprintf(stderr, "Type error: && requires bool operands\n"); exit(1);
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
              fprintf(stderr, "Type error: ! requires bool operand\n"); exit(1);
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
                      node_type($1), node_type($3)); exit(1);
          }
          ASTNode *n = create_node("Condition: EQ<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | equality_expression NOT_EQUAL relational_expression
      {
          if (strcmp(node_type($1), node_type($3)) != 0) {
              fprintf(stderr, "Type error: cannot compare <%s> with <%s>\n",
                      node_type($1), node_type($3)); exit(1);
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
              fprintf(stderr, "Type error: relational operator on non-numeric type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: LT<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | relational_expression GREATER additive_expression
      {
          if (strcmp(node_type($1),"string")==0||strcmp(node_type($1),"bool")==0||
              strcmp(node_type($3),"string")==0||strcmp(node_type($3),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: GT<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | relational_expression GREATER_EQUAL additive_expression
      {
          if (strcmp(node_type($1),"string")==0||strcmp(node_type($1),"bool")==0||
              strcmp(node_type($3),"string")==0||strcmp(node_type($3),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: GE<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | relational_expression LESS_EQUAL additive_expression
      {
          if (strcmp(node_type($1),"string")==0||strcmp(node_type($1),"bool")==0||
              strcmp(node_type($3),"string")==0||strcmp(node_type($3),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: LE<bool>");
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | additive_expression  { $$ = $1; }
    ;

additive_expression
    : additive_expression PLUS term
      {
          char buf[64]; sprintf(buf, "Arith: Plus<%s>", arith_type($1,$3));
          ASTNode *n = create_node(buf);
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | additive_expression MINUS term
      {
          char buf[64]; sprintf(buf, "Arith: Minus<%s>", arith_type($1,$3));
          ASTNode *n = create_node(buf);
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | term  { $$ = $1; }
    ;

term
    : term MUL factor
      {
          char buf[64]; sprintf(buf, "Arith: Mult<%s>", arith_type($1,$3));
          ASTNode *n = create_node(buf);
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | term DIV factor
      {
          char buf[64]; sprintf(buf, "Arith: Div<%s>", arith_type($1,$3));
          ASTNode *n = create_node(buf);
          n->child1 = $1; n->child2 = $3; $$ = n;
      }
    | factor  { $$ = $1; }
    ;

/* ════════════════════════════════════════════
   FACTOR — now includes func_call
   ──────────────────────────────────────────
   After NAME, the lookahead resolves:
     '('  →  func_call  (shift into func_call)
     else →  variable   (reduce NAME → factor)
   '(' is never in follow(factor), so zero conflicts.
   ════════════════════════════════════════════ */
factor
    : func_call
      { $$ = $1; }
    | NAME
      {
          char buf[256];
          sprintf(buf, "Name: %s_<%s>", $1, sym_lookup($1));
          free($1); $$ = create_node(buf);
      }
    | INT_NUM
      {
          char buf[128]; sprintf(buf, "Num: %d<int>", atoi($1));
          $$ = create_node(buf); free($1);
      }
    | FLOAT_NUM
      {
          char buf[128]; sprintf(buf, "Num: %.2f<float>", atof($1));
          $$ = create_node(buf); free($1);
      }
    | STR_CONST
      {
          char buf[128]; sprintf(buf, "Str: %s<string>", $1);
          $$ = create_node(buf); free($1);
      }
    | BOOL_CONST
      {
          char buf[128]; sprintf(buf, "Bool: %s<bool>", $1);
          $$ = create_node(buf); free($1);
      }
    | LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET
      { $$ = $2; }
    | MINUS factor %prec UMINUS
      {
          if (strcmp(node_type($2),"string")==0||strcmp(node_type($2),"bool")==0) {
              fprintf(stderr, "Type error: unary minus on non-numeric type\n"); exit(1);
          }
          char buf[64]; sprintf(buf, "Arith: Uminus<%s>", node_type($2));
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
