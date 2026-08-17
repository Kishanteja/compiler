sclp: parser.tab.c lex.yy.c main.cpp ast.hpp symtab.hpp tac.hpp rtl.hpp tokens.hpp
	g++ -o sclp parser.tab.c lex.yy.c main.cpp

parser.tab.c parser.tab.h: parser.y
	bison -dv parser.y

lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h parser.output sclp *.toks *.ast *.tac *.rtl