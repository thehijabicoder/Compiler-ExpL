%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "gram1.c"

    int yylex(void);
    void yyerror(char const* s);
    extern FILE *yyin;

%}

%union {
    int val;
    struct node *n;
}

%token FUN VAR NUM IF ELSE DO WHILE READ WRITE GE LE EQ NE
%type <n> Program Stmts Stmt AsgStmt IfStmt IfElseStmt WhileStmt ReadStmt WriteStmt VAR E NUM 
%nonassoc GE LE '<' '>' EQ NE
%left '+' '-'
%left '*'

%%

Program : FUN '('')' '{' Stmts '}' {
    fp = fopen("B190420CS.xsm", "w+");
    fprintf(fp, "0\n2056\n0\n0\n0\n0\n0\n0\nMOV SP,4095\nPUSH R0\nCALL 2064\nINT 10\n");

    generate_code($5);
    fprintf(fp, "RET\n");
    fclose(fp);

    symbolTable *temp = global_symbol_table_head;
    while(temp != NULL) {
        global_symbol_table_head = global_symbol_table_head->next;
        free(temp);
        temp = global_symbol_table_head;
    }

    exit(0);
} ;

Stmts : Stmts Stmt {$$ = node_create(STMTS, -1, NULL, $1, $2, NULL);}
    |Stmt {$$ =$1;};

Stmt : AsgStmt {$$ = $1;} 
    | IfStmt {$$ = $1;}
    | IfElseStmt {$$ = $1;}
    | WhileStmt {$$ = $1;}
    | ReadStmt {$$ = $1;}
    | WriteStmt {$$ = $1;};

AsgStmt : VAR '=' E ';' {
    symbolTable *temp = find_var($1->name);
    if(!temp)
        install($1->name);
    $$ = node_create(ASGSTMT, -1, NULL, $1, $3, NULL);
};

IfStmt : IF '('E')' '{' Stmts '}' {$$ = node_create(IFSTMT, -1, NULL, $3, $6, NULL);};

IfElseStmt : IF '('E')' '{' Stmts '}' ELSE '{' Stmts '}' {$$ = node_create(IFSTMT, -1, NULL, $3, $6, $10);};

WhileStmt : DO '{' Stmts '}' WHILE '('E')' ';' {$$ = node_create(WHILESTMT, -1, NULL, $3, $7, NULL);};

ReadStmt : READ '('VAR')' ';' {
    symbolTable *temp = find_var($3->name);
    if(!temp)
        install($3->name);
    $$ = node_create(READSTMT, -1, NULL, $3, NULL, NULL);
};

WriteStmt : WRITE '('E')' ';' {$$ = node_create(WRITESTMT, -1, NULL, $3, NULL, NULL);};

E : E '+' E {$$ = internal_node_create(PLUS, $1, $3);}
| E '-' E {$$ = internal_node_create(MINUS, $1, $3);}
| E '*' E {$$ = internal_node_create(MUL, $1, $3);}
| E '<' E {$$ = internal_node_create(LT, $1, $3);}
| E '>' E {$$ = internal_node_create(GT, $1, $3);}
| E LE E {$$ = internal_node_create(LE, $1, $3);}
| E GE E {$$ = internal_node_create(GE, $1, $3);}
| E EQ E {$$ = internal_node_create(EQ, $1, $3);}
| E NE E {$$ = internal_node_create(NE, $1, $3);}
| '('E')' {$$ = $2;}
| VAR {$$ = $1;} 
| NUM {$$ = $1;} ;

%%

void yyerror(char const *s)
{
    printf("yyerror %s",s);
}

int main(int argc, char *argv[]) {
	if(argc>1)
	{
    	FILE *fp = fopen(argv[1],"r");
    	if(fp)
        	yyin = fp;
	}
	yyparse();

	return 0;
}
