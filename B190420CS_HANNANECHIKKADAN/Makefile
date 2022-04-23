all: label.l B190420CS.xsm
	lex -o label.yy.c label.l
	gcc label.yy.c -o linker
	-./linker

B190420CS.xsm: compiler
	-./compiler $(input)

compiler: y.tab.c lex.yy.c
	gcc y.tab.c lex.yy.c -o compiler

y.tab.c y.tab.h: gram1.y
	yacc -d gram1.y

lex.yy.c: gram1.l
	lex gram1.l
	
clean:
	rm -f *.yy.c *.xsm compiler linker tempfile *.tab.* *.txt
