%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "y.tab.h"
    #include "gram1.h"
    
    void yyerror(char const* s);
%}

%%

[ \t\n] {;}

[0-9]+ {
    yylval.n = node_create(NUM, atoi(yytext), NULL, NULL, NULL, NULL);
    return NUM;
}

"FUN" {
    return FUN;
}
"if" {return IF;}
"else" {return ELSE;}
"do" {return DO;}
"while" {return WHILE;}
"read" {return READ;}
"write" {return WRITE;}
">=" {return GE;}
"<=" {return LE;}
"==" {return EQ;}
"!=" {return NE;}
"+" {return *yytext;}
"-" {return *yytext;}
"*" {return *yytext;}
"<" {return *yytext;}
">" {return *yytext;}
"=" {return *yytext;}
"(" {return *yytext;}
")" {return *yytext;}
"{" {return *yytext;}
"}" {return *yytext;}
";" {return *yytext;}

[a-zA-Z][a-zA-Z0-9]* {

	char *variable_name = (char *) malloc (30);
	strcpy(variable_name, yytext);

	yylval.n = node_create(VAR, -1, variable_name, NULL, NULL, NULL);

	return VAR;
}

. {
    printf("Unknown character: %s\n", yytext);yyerror("unknown character\n");
    exit(1);
}

%%

int yywrap(void)
{
    return 1;
}
