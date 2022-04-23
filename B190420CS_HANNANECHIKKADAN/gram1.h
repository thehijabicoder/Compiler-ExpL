#define STMTS 1
#define ASGSTMT 2
#define IFSTMT 3
#define WHILESTMT 4
#define READSTMT 5
#define WRITESTMT 6
#define PLUS 7
#define MINUS 8
#define MUL 9
#define LT 10
#define GT 11

typedef struct symbolTable
{
    char *name;
    int binding_address;
    struct symbolTable *next;
} symbolTable;

typedef struct node
{
    int type;
    char *name;
    int val;
    struct node *child1, *child2, *child3;
    symbolTable *entry;
} node;

node *node_create(int type, int val, char *name, node *child1, node *child2, node *child3);

node *internal_node_create(int type, node *left, node *right);

symbolTable *find_var(char *name);

void install(char *name);

int generate_code(node *p);
