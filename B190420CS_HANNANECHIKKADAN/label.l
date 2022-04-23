%{
    #include <stdio.h>
    int instcnt = 2048, passcount=0;
    FILE *fp, *yyin;

    typedef struct labeltable
	{
		int val;
		int address;
		struct labeltable *next;
	}label;

    label *label_table_head=NULL,*label_table_tail=NULL;
    
    int searchLabel(int labelnum);
    void insertLabel(int labelnum);


%}

%%

"0\n2056\n0\n0\n0\n0\n0\n0\n" {
    fprintf(fp, "%s", yytext);
    instcnt = instcnt + 8;
}

L[0-9]+\n {
    if(passcount == 0)
    {
        fprintf(fp, "%s", yytext);
        instcnt += 2;
    }
    else
    {
        fprintf(fp, "%d\n", searchLabel(atoi(yytext + 1)));
    }
}

L[0-9]+:\n {

    insertLabel(atoi(yytext + 1));

}

[\n]+ {
    fprintf(fp, "%s", yytext);
    instcnt += 2;
}

. {
    fprintf(fp, "%c", *yytext);
}

%%

void insertLabel(int labelnum)
{
	label *temp = (label*)malloc(sizeof(label));
	temp->val = labelnum;
	temp->address = instcnt;
	temp->next = NULL;
	if(label_table_head == NULL)
	{
		label_table_head = temp;
		label_table_tail = temp;
	}
	else
	{
		label_table_tail->next = temp;
		label_table_tail = temp;
	}

}

int searchLabel(int labelnum)
{
    label *temp = label_table_head;
   	while(temp != NULL)
   	{
    	if(temp->val == labelnum)
    	{
    		return temp->address;
    	}
    	else
    	{
    		temp=temp->next;
    	}
    }
}

int yywrap()
{
    if(passcount == 0)
    {
        fflush(fp);
        fclose(fp);
        yyin = fopen("tempfile","r");
        fp = fopen("B190420CS.xsm","w");
        passcount = 1;
        yylex();
        fflush(fp);
        fclose(fp);
        return 0;
    }

    return 1;
}

int main()
{
    yyin = fopen("B190420CS.xsm","r");
    fp = fopen("tempfile","w");
    yylex();
    return 0;
}
