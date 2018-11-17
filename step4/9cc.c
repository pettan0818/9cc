#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Set property for token types.
enum {
    TK_NUM = 256, // INT token
};

// token types
typedef struct {
    int ty; // token type(maybe contains TK_NUM or TK_EOF.)
    int val; // if ty is TK_NUM then that int.
} Token;

// Node type.
// Node has type(includes operator or num) and left hand side / right hand side and value.
typedef struct Node {
    int ty;  // operator or number(ND_NUM).
    struct Node *lhs;  // left-hand side
    struct Node *rhs;  // right-hand side.
    int val;  // contains value of number.
} Node;

// Make new node. (operator)
// initialize node struct.
// 1. Allocate memory.
// 2. bind pointer to member vals.
Node *new_node(int op, Node *lhs, Node *rhs) {  // return value is pointer, so Node *<func name> is declared here.
    Node *node = malloc(sizeof(Node));
    // use arrow operator(->), when you want to access a member variable of pointer.
    node->ty = op;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

// Make new node contained number.
Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}



// tokenized result should be saved to this array.
// We conveive num of tokens are below 100.
Token tokens[100]; // 構造体100個

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
void error(int i) {
    fprintf(stderr, "Unexpected token: %s\n", tokens[i].input);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid num of args.\n");
        return 1;
    }

    // do tokenize.
    tokenize(argv[1]);
    Node* node = expr();

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

