%option noyywrap
%option yylineno

%{
#include "parser.tab.h"
#include "tokens.hpp"
#include <cstdio>
#include <cstring>

extern int yylineno;
extern char *yytext;
%}

%x COMMENT

%%

"//".*                   ;
"/*"                     { BEGIN(COMMENT); }
<COMMENT>"*/"            { BEGIN(INITIAL); }
<COMMENT>\n              ;
<COMMENT>.               ;

[ \t\r\n]+               ;

"int"    { buffer_token("INTEGER", yytext, yylineno);  return INTEGER; }
"float"  { buffer_token("FLOAT",   yytext, yylineno);  return FLOAT;   }
"string" { buffer_token("STRING",  yytext, yylineno);  return STRING;  }
"void"   { buffer_token("VOID",    yytext, yylineno);  return VOID;    }
"bool"   { buffer_token("BOOL",    yytext, yylineno);  return BOOL;    }
"read"   { buffer_token("READ",    yytext, yylineno);  return READ;    }
"print"  { buffer_token("WRITE",   yytext, yylineno);  return WRITE;   }
"if" { buffer_token("IF", yytext, yylineno); return IF;}
"else" { buffer_token("ELSE", yytext, yylineno); return ELSE;}
"while" { buffer_token("WHILE", yytext, yylineno); return WHILE;}
"do" { buffer_token("DO", yytext, yylineno); return DO;}

"true"|"false" {
    yylval.str = strdup(yytext);
    buffer_token("BOOL_CONST", yytext, yylineno);
    return BOOL_CONST;
}

\"[^\n\"]*\" {
    yylval.str = strdup(yytext);
    buffer_token("STR_CONST", yytext, yylineno);
    return STR_CONST;
}

[0-9]+"."[0-9]+ {
    yylval.str = strdup(yytext);
    buffer_token("FLOAT_NUM", yytext, yylineno);
    return FLOAT_NUM;
}

[0-9]+ {
    yylval.str = strdup(yytext);
    buffer_token("INT_NUM", yytext, yylineno);
    return INT_NUM;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str = strdup(yytext);
    buffer_token("NAME", yytext, yylineno);
    return NAME;
}

"?"  { buffer_token("QUESTION_MARK", yytext, yylineno); return QUESTION_MARK; }
":"  { buffer_token("COLON",         yytext, yylineno); return COLON;         }
"||" { buffer_token("OR",            yytext, yylineno); return OR;            }
"&&" { buffer_token("AND",           yytext, yylineno); return AND;           }
"!"  { buffer_token("NOT",           yytext, yylineno); return NOT;           }
"==" { buffer_token("EQUAL",         yytext, yylineno); return EQUAL;         }
"!=" { buffer_token("NOT_EQUAL",     yytext, yylineno); return NOT_EQUAL;     }
">=" { buffer_token("GREATER_EQUAL", yytext, yylineno); return GREATER_EQUAL; }
">"  { buffer_token("GREATER",       yytext, yylineno); return GREATER;       }
"<=" { buffer_token("LESS_EQUAL",    yytext, yylineno); return LESS_EQUAL;    }
"<"  { buffer_token("LESS",          yytext, yylineno); return LESS;          }
"="  { buffer_token("ASSIGN_OP",     yytext, yylineno); return ASSIGN_OP;     }
"+"  { buffer_token("PLUS",          yytext, yylineno); return PLUS;          }
"-"  { buffer_token("MINUS",         yytext, yylineno); return MINUS;         }
"*"  { buffer_token("MUL",           yytext, yylineno); return MUL;           }
"/"  { buffer_token("DIV",           yytext, yylineno); return DIV;           }
";"  { buffer_token("SEMICOLON",     yytext, yylineno); return SEMICOLON;     }
","  { buffer_token("COMMA",         yytext, yylineno); return COMMA;         }
"("  { buffer_token("LEFT_ROUND_BRACKET",  yytext, yylineno); return LEFT_ROUND_BRACKET;  }
")"  { buffer_token("RIGHT_ROUND_BRACKET", yytext, yylineno); return RIGHT_ROUND_BRACKET; }
"{"  { buffer_token("LEFT_CURLY_BRACKET",  yytext, yylineno); return LEFT_CURLY_BRACKET;  }
"}"  { buffer_token("RIGHT_CURLY_BRACKET", yytext, yylineno); return RIGHT_CURLY_BRACKET; }

. {
    fprintf(stderr, "Lexical error: invalid character '%s' at line %d\n",
            yytext, yylineno);
    exit(1);
}

%%