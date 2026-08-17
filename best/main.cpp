#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "ast.hpp"
#include "tac.hpp"
#include "rtl.hpp"
#include "tokens.hpp"

extern FILE *yyin;
extern int   yyparse();
extern int   yylex(void);

int show_tokens = 0;
int sa_scan     = 0;
int sa_parse    = 0;
int sa_ast      = 0;
int sa_tac      = 0;
int show_ast    = 0;
int show_tac    = 0;
int show_rtl    = 0;

FILE *tok_file = nullptr;
FILE *ast_file = nullptr;
FILE *tac_file = nullptr;
FILE *rtl_file = nullptr;

void process_command_options(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "--show-tokens") == 0) show_tokens = 1;
        else if (strcmp(argv[i], "--sa-scan")     == 0) sa_scan     = 1;
        else if (strcmp(argv[i], "--sa-parse")    == 0) sa_parse    = 1;
        else if (strcmp(argv[i], "--sa-ast")      == 0) sa_ast      = 1;
        else if (strcmp(argv[i], "--sa-tac")      == 0) sa_tac      = 1;
        else if (strcmp(argv[i], "--show-ast")    == 0) show_ast    = 1;
        else if (strcmp(argv[i], "--show-tac")    == 0) show_tac    = 1;
        else if (strcmp(argv[i], "--show-rtl")    == 0) show_rtl    = 1;
        else {
            yyin = fopen(argv[i], "r");
            if (!yyin) { perror("File open error"); exit(1); }
            if (show_tokens) {
                char fname[256];
                sprintf(fname, "%s.toks", argv[i]);
                tok_file = fopen(fname, "w");
            }
            if (show_ast) {
                char fname[256];
                sprintf(fname, "%s.ast", argv[i]);
                ast_file = fopen(fname, "w");
            }
            if (show_tac) {
                char fname[256];
                sprintf(fname, "%s.tac", argv[i]);
                tac_file = fopen(fname, "w");
            }
            if (show_rtl) {
                char fname[256];
                sprintf(fname, "%s.rtl", argv[i]);
                rtl_file = fopen(fname, "w");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    process_command_options(argc, argv);

    int status = 0;

    if (sa_scan) {
        while (yylex()) {}
    } else if (sa_ast || sa_tac || sa_parse || !sa_scan) {
        status = yyparse();
    }

    if (status == 0 && show_tokens && tok_file)
        print_toks(tok_file);

    if (status == 0 && show_ast && ast_file) {
        char *buf=NULL; size_t sz=0;
        FILE *mem=open_memstream(&buf,&sz);
        print_ast(mem); fclose(mem);
        fprintf(ast_file,"%s",buf); free(buf);
    }

    if (status == 0 && show_tac && tac_file) {
        char *buf=NULL; size_t sz=0;
        FILE *mem=open_memstream(&buf,&sz);
        print_tac(mem); fclose(mem);
        fprintf(tac_file,"%s",buf); free(buf);
    }

    if (status == 0 && show_rtl && rtl_file) {
        char *buf=NULL; size_t sz=0;
        FILE *mem=open_memstream(&buf,&sz);
        print_rtl(mem); fclose(mem);
        fprintf(rtl_file,"%s",buf); free(buf);
    }

    return status;
}