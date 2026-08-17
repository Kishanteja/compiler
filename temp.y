program
	: global_decl_statement_list func_def_list                                               (rule 1)
	| func_def_list                                                                          (rule 2)
;

global_decl_statement_list
	: global_decl_statement_list func_decl                                                   (rule 3)
	| global_decl_statement_list var_decl_stmt                                               (rule 4)
	| var_decl_stmt                                                                          (rule 5)
	| func_decl                                                                              (rule 6)
;

func_decl
	: func_header '(' formal_param_list ')' ';'                                              (rule 7)
	| func_header '('')' ';'                                                                 (rule 8)
;

func_def_list
	: func_def_list func_def                                                                 (rule 9)
	| func_def                                                                               (rule 10)
;

func_header
	: named_type NAME                                                                        (rule 11)
;

func_def
	: func_header '('formal_param_list')' '{'optional_local_var_decl_stmt_list statement_list'}'(rule 12)
	| func_header '(' ')' '{' optional_local_var_decl_stmt_list statement_list '}'           (rule 13)
;

formal_param_list
	: formal_param_list ',' formal_param                                                     (rule 14)
	| formal_param                                                                           (rule 15)
;

formal_param
	: param_type NAME                                                                        (rule 16)
;

param_type
	: INTEGER                                                                                (rule 17)
	| FLOAT                                                                                  (rule 18)
	| BOOL                                                                                   (rule 19)
	| STRING                                                                                 (rule 20)
;

statement_list
	: statement_list statement                                                               (rule 21)
	| %empty                                                                                 (rule 22)
;

statement
	: assignment_statement                                                                   (rule 23)
	| if_statement                                                                           (rule 24)
	| do_while_statement                                                                     (rule 25)
	| while_statement                                                                        (rule 26)
	| compound_statement                                                                     (rule 27)
	| print_statement                                                                        (rule 28)
	| read_statement                                                                         (rule 29)
	| call_statement                                                                         (rule 30)
	| return_statement                                                                       (rule 31)
; 

call_statement
	: func_call ';'                                                                          (rule 32)
;

func_call
	: NAME '(' actual_arg_list ')'                                                           (rule 33)
;

actual_arg_list
	: non_empty_arg_list                                                                     (rule 34)
	| %empty                                                                                 (rule 35)
;

non_empty_arg_list
	: non_empty_arg_list ',' actual_arg                                                      (rule 36)
	| actual_arg                                                                             (rule 37)
;

actual_arg
	: expression                                                                             (rule 38)
;

return_statement
	: RETURN expression ';'                                                                  (rule 39)
;

optional_local_var_decl_stmt_list
	: %empty                                                                                 (rule 40)
	| var_decl_stmt_list                                                                     (rule 41)
;

var_decl_stmt_list
	: var_decl_stmt                                                                          (rule 42)
	| var_decl_stmt_list var_decl_stmt                                                       (rule 43)
;

var_decl_stmt
	: named_type var_decl_item_list ';'                                                      (rule 44)
;

var_decl_item_list
	: var_decl_item_list ',' var_decl_item                                                   (rule 45)
	| var_decl_item                                                                          (rule 46)
;

var_decl_item
	: NAME                                                                                   (rule 47)
	| NAME array_decl                                                                        (rule 48)
	| pointer_decl NAME                                                                      (rule 49)
;
pointer_decl
	: '*'                                                                                    (rule 50)
	| '*' pointer_decl                                                                       (rule 51)
;

array_decl
	:   '[' INTEGER_NUMBER  ']'                                                              (rule 52)
	|   '[' INTEGER_NUMBER  ']' array_decl                                                   (rule 53)
;

named_type
	: INTEGER                                                                                (rule 54)
	| FLOAT                                                                                  (rule 55)
	| VOID                                                                                   (rule 56)
	| STRING                                                                                 (rule 57)
	| BOOL                                                                                   (rule 58)
;

assignment_statement
	: variable_as_operand ASSIGN expression ';'                                              (rule 59)
	| variable_as_operand ASSIGN func_call ';'                                               (rule 60)
	| variable_as_operand ASSIGN ADDRESSOF variable_name ';'                                 (rule 61)
;

if_condition
	: '(' expression ')'                                                                     (rule 62)
;

if_statement
	: IF if_condition statement ELSE statement                                               (rule 63)
	| IF if_condition statement                                                              (rule 64)
;

do_while_statement
	: DO statement WHILE '(' expression ')' ';'                                              (rule 65)
;

while_statement
	: WHILE '(' expression ')' statement                                                     (rule 66)
;

compound_statement
	: '{' statement_list '}'                                                                 (rule 67)
; 

print_statement
	: WRITE expression ';'                                                                   (rule 68)

;

read_statement
	: READ variable_name ';'                                                                 (rule 69)

;

expression
	: expression '+' expression                                                              (rule 70)
	| expression '-' expression                                                              (rule 71)
	| expression '*' expression                                                              (rule 72)
	| expression '/' expression                                                              (rule 73)
	| '-' expression                                                                         (rule 74)
	| '(' expression ')'                                                                     (rule 75)
	| expression '?' expression ':' expression                                               (rule 76)
	| expression AND expression                                                              (rule 77)
	| expression OR expression                                                               (rule 78)
	| NOT expression                                                                         (rule 79)
	| rel_expression                                                                         (rule 80)
	| variable_as_operand                                                                    (rule 81)
	| constant_as_operand                                                                    (rule 82)
;

rel_expression
	: expression LT expression                                                               (rule 83)
	| expression LE expression                                                               (rule 84)
	| expression GT expression                                                               (rule 85)
	| expression GE expression                                                               (rule 86)
	| expression NE expression                                                               (rule 87)
	| expression EQ expression                                                               (rule 88)
;

variable_as_operand
	: variable_name                                                                          (rule 89)
	| array_access                                                                           (rule 90)
	| pointer_access                                                                         (rule 91)
;

variable_name
	: NAME                                                                                   (rule 92)
;

array_access
	: variable_name array_dimensions                                                         (rule 93)
;

pointer_access
	: '*' variable_name                                                                      (rule 94)
	| '*' pointer_access                                                                     (rule 95)
;

array_dimensions
	: '[' expression ']'                                                                     (rule 96)
	| array_dimensions '[' expression ']'                                                    (rule 97)
;

constant_as_operand
	: INTEGER_NUMBER                                                                         (rule 98)
	| DOUBLE_NUMBER                                                                          (rule 99)
	| STRING_CONSTANT                                                                        (rule 100)
;
