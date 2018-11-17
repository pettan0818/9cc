#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Set property for token type.
enum {
    TK_NUM = 256, // INT token
    TK_EOF,  // EOF token
};

// Set property for node types.
enum {
    ND_NUM = 256, // INT token
};

// token types
typedef struct {
    int ty; // token type(maybe contains TK_NUM or TK_EOF.)
    int val; // if ty is TK_NUM then that int.
    char *input; // token chars.
} Token;

// Node type.
// Node has type(includes operator or num) and left hand side / right hand side and value.
typedef struct Node {
    int ty;  // operator or number(ND_NUM).
    struct Node *lhs;  // left-hand side
    struct Node *rhs;  // right-hand side.
    int val;  // contains value of number.
} Node;

// Prototype
Node *term(int pos);
Node *mul(int pos);
Node *expr(int pos);
void tokenize(char *p);

// tokenized result should be saved to this array.
// We conveive num of tokens are below 100.
Token tokens[100];

// Make new node. (operator)
// initialize node struct.
// 1. Allocate memory.
// 2. bind pointer to member vals.
Node *new_node(int op, Node *lhs, Node *rhs) {  // return value is pointer, so Node *<func name> is declared here.
    Node *node = malloc(sizeof(Node));
    // use arrow operator(->), when you want to access a member variable of pointer.
    node->ty = op;  // lhs *op* rhs
    node->lhs = lhs;  // *lhs* op rhs
    node->rhs = rhs;  // lhs op *rhs*

    return node;  // return node pointer.
}

// Make new node contained number.
Node *new_node_num(int val) {  // the lowest node of AST
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;  // type is num that is defined above.
    node->val = val;
    return node;
}

// chars which are contained at *p will be divided and saved in tokens.
void tokenize(char *p) {
    int i = 0; // counter for tokens num.
    while (*p) {
        // Skip the blank chars.
        if (isspace(*p)) {
            p++;  // address of *p is increased by 1.
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].ty = *p;  // implies +- will be type.
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "Unable to toknize: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

// Error Reporter.
void error(char *body, char *parameter) {
    fprintf(stderr, body, parameter);
    exit(1);
}

// load term. term is defined below.
// term: number | "(" expr ")"
// This rule can be converted like below. And we use [None] as nothing.
// term: number | (expr')
Node *term(int pos) {
    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    }
    if (tokens[pos].ty == '(') {
        pos++;
        Node *node = expr(pos); // inside () parsed again as expr
        if (tokens[pos].ty != ')'){  // if tokens don't include ')', raise error.
            error("[Fatal] couldn't parse ')': %s", tokens[pos].input);
        }
        pos++;
        return node;
    }
    error("[Fatal] illegal<not number of not terms> input found.: %s", tokens[pos].input);
}

// load mul. mul is defined below.
// mul: term | term "*" mul | term "/" mul
// This rule can be converted like below. And we use [None] as nothing.
// mul: term mul'
// mul': [None] | "*" term | "/" term
// この生成規則に従うと、mulは、空の記号列か* termか/ termに展開されることになる。
Node *mul(int pos) {
    Node *lhs = term(pos);  //生成規則上、termが優先されるため、とりあえずtermに投げ込む。

    if (tokens[pos].ty == '*') {
        pos++;
        return new_node('*', lhs, mul(pos));
    }
    if(tokens[pos].ty == '/') {
        pos++;
        return new_node('/', lhs, mul(pos));
    }
    return lhs;
}

// load expr. expr is defined below.
// expr: mul | mul "+" expr | mul "-" expr
// This rule can be converted like below. And we use [None] as nothing.
// expr: mul expr'
// expr': [None] | "+" expr | "-" expr
// この生成規則に従うと、exprは、空の記号列か+ exprか- exprに展開されることになる。
Node *expr(int pos) {
    Node *lhs = mul(pos);  // 優先的にパースされるべきmulやtermが先に展開された上でそれらを全て展開した結果が返ってくる。

    if (tokens[pos].ty == '+') {
        pos++;
        return new_node('+', lhs, expr(pos));
    }
    if (tokens[pos].ty == '-') {
        pos++;
        return new_node('-', lhs, expr(pos));
    }
    return lhs;
}

void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }
    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty){
        case '+':
            printf("  add rax, rdi\n");
            break;
        case '-':
            printf("  sub rax, rdi\n");
            break;
        case '*':
            printf("  mul rdi\n");
            break;
        case '/':
            printf("  mov rdx, 0\n");
            printf("  div rdi\n");
    }
    printf("  push rax\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid num of args.\n");
        return 1;
    }

    int pos = 0;

    // do tokenize.
    tokenize(argv[1]);
    Node* node = expr(pos);

    // Output former half of assembly.
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // By following AST, generate codes.
    gen(node);

    // Because we use stack machine, the result of exed formula should remain the top of the stack.
    // Load this value to RAX, and then make it the return value of func.
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}

