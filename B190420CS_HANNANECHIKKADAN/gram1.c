#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "gram1.h"

FILE *fp = NULL;

symbolTable *global_symbol_table_head = NULL;

symbolTable *global_symbol_table_tail = NULL;

node *node_create(int type, int val, char *name, node *child1, node *child2, node *child3)
{
    node *temp;
    temp = (node *)malloc(sizeof(node));
    temp->type = type;
    temp->val = val;
    if (name != NULL)
    {
        temp->name = name;
    }
    else
        temp->name = NULL;

    temp->child1 = child1;
    temp->child2 = child2;
    temp->child3 = child3;
    return temp;
}

node *internal_node_create(int type, node *left, node *right)
{
    node *temp;
    temp = node_create(type, -1, NULL, left, right, NULL);
    return temp;
}

int labelcnt = -1;
int reg_to_use = -1;
int next_reg;

int allocate_reg()
{
    if (reg_to_use < 7)
    {
        reg_to_use++;
        return reg_to_use;
    }

    printf("Error: No more registers available\n");
    exit(0);
}

void deallocate_reg()
{
    if (reg_to_use > -1)
    {
        reg_to_use--;
    }
    else
    {
        printf("Error: Deallocating Register Error\n");
    }
}

symbolTable *find_var(char *name)
{
    symbolTable *temp = global_symbol_table_head;
    while (temp != NULL)
    {

        if (strncmp(temp->name, name, strlen(temp->name)) == 0)
        {
            return temp;
        }
        temp = temp->next;
    }
    return temp;
}

int address = 5118;

void install(char *name)
{

    symbolTable *new_symbol = find_var(name);
    if (new_symbol != NULL)
    {
        printf("Variable %s has already been declared, exiting.\n", name);
        exit(0);
    }

    new_symbol = (symbolTable *)malloc(sizeof(symbolTable));
    new_symbol->name = name;
    new_symbol->next = NULL;
    new_symbol->binding_address = address;
    address -= 2;

    if (global_symbol_table_tail == NULL)
    {
        global_symbol_table_tail = new_symbol;
        global_symbol_table_head = new_symbol;
    }
    else
    {
        global_symbol_table_tail->next = new_symbol;
        global_symbol_table_tail = new_symbol;
    }
}

int generate_code(node *p)
{
    int lhs, rhs, label1, label2, start_of_loop_c, end_of_loop_c;

    symbolTable *symbol_table_entry;

    switch (p->type)
    {
    case STMTS:
        generate_code(p->child1);
        generate_code(p->child2);
        return -1;
    case ASGSTMT:
        rhs = generate_code(p->child2);
        symbol_table_entry = find_var(p->child1->name);
        fprintf(fp, "MOV [%d], R%d\n", symbol_table_entry->binding_address, rhs);
        deallocate_reg();
        break;
    case IFSTMT:
        lhs = generate_code(p->child1);
        labelcnt++;
        label1 = labelcnt;
        fprintf(fp, "JZ R%d, L%d\n", lhs, label1);
        generate_code(p->child2);
        if (p->child3)
        {
            labelcnt++;
            label2 = labelcnt;
            fprintf(fp, "JMP L%d\n", label2);
            fprintf(fp, "L%d:\n", label1);
            generate_code(p->child3);
            fprintf(fp, "L%d:\n", label2);
        }
        else
        {
            fprintf(fp, "L%d:\n", label1);
        }
        deallocate_reg();
        return -1;
    case WHILESTMT:
        labelcnt++;
        start_of_loop_c = labelcnt;
        fprintf(fp, "L%d:\n", start_of_loop_c);
        generate_code(p->child1);
        lhs = generate_code(p->child2);
        labelcnt++;
        end_of_loop_c = labelcnt;
        fprintf(fp, "JZ R%d, L%d\n", lhs, end_of_loop_c);
        fprintf(fp, "JMP L%d\n", start_of_loop_c);
        fprintf(fp, "L%d:\n", end_of_loop_c);
        return -1;
    case READSTMT:
        symbol_table_entry = find_var(p->child1->name);
        lhs = allocate_reg();
        fprintf(fp, "MOV R%d, 7\n", lhs);
        fprintf(fp, "PUSH R%d\n", lhs);
        fprintf(fp, "MOV R%d, -1\n", lhs);
        fprintf(fp, "PUSH R%d\n", lhs);
        fprintf(fp, "MOV R%d, %d\n", lhs, symbol_table_entry->binding_address);
        fprintf(fp, "PUSH R%d\n", lhs);
        fprintf(fp, "PUSH R%d\n", lhs);
        fprintf(fp, "PUSH R%d\n", lhs);
        fprintf(fp, "INT 6\n");
        fprintf(fp, "POP R%d\n", lhs);
        fprintf(fp, "POP R%d\n", lhs);
        fprintf(fp, "POP R%d\n", lhs);
        fprintf(fp, "POP R%d\n", lhs);
        fprintf(fp, "POP R%d\n", lhs);
        deallocate_reg();
        break;
    case WRITESTMT:
        lhs = generate_code(p->child1);
        rhs = allocate_reg();
        fprintf(fp, "MOV R%d, 5\n", rhs);
        fprintf(fp, "PUSH R%d\n", rhs);
        fprintf(fp, "MOV R%d, -2\n", rhs);
        fprintf(fp, "PUSH R%d\n", rhs);
        fprintf(fp, "PUSH R%d\n", lhs);
        fprintf(fp, "PUSH R%d\n", rhs);
        fprintf(fp, "PUSH R%d\n", rhs);
        fprintf(fp, "INT 7\n");
        fprintf(fp, "POP R%d\n", rhs);
        fprintf(fp, "POP R%d\n", rhs);
        fprintf(fp, "POP R%d\n", rhs);
        fprintf(fp, "POP R%d\n", rhs);
        fprintf(fp, "POP R%d\n", rhs);
        deallocate_reg();
        break;
    case PLUS:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "ADD R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case MINUS:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "SUB R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case MUL:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "MUL R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case LT:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "LT R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case GT:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "GT R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case EQ:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "EQ R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case NE:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "NE R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case GE:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "GE R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case LE:
        lhs = generate_code(p->child1);
        rhs = generate_code(p->child2);
        fprintf(fp, "LE R%d, R%d\n", lhs, rhs);
        deallocate_reg();
        return lhs;
    case NUM:
        next_reg = allocate_reg();
        fprintf(fp, "MOV R%d, %d\n", next_reg, p->val);
        return next_reg;
        break;
    case VAR:
        lhs = allocate_reg();
        symbol_table_entry = find_var(p->name);
        fprintf(fp, "MOV R%d, [%d]\n", lhs, symbol_table_entry->binding_address);
        return lhs;
        break;
    }
}
