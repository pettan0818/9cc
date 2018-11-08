#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Set property for token types.
enum {
    TK_NUM = 256, // INT token
    TK_EOF,  // End Of File(Input) token
}

// token types
typedef struct {
    int ty; // token type(maybe contains TK_NUM or TK_EOF.)
    int val; // if ty is TK_NUM then that int.
    char *input; // tokenize string (for error message.)
} Token;

// tokenized result should be saved to this array.
// We conveive num of tokens are below 100.
Token tokens[100] // 構造体100個

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
            // p++;
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

    // Output former half of assembly.
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // The first part of formula must be number and if it is OK -> output first mov order.
    if (tokens[0].ty != TK_NUM) {
        error(0); // guard
    }
    printf("  mov rax, %d\n", tokens[0].val);

    // '+ <number>' or '- <number>' consumed and then put assembly.
    int i = 1;
    while (tokens[i].ty != TK_EOF) {
        if (tokens[i].ty == '+') {
            i++;  // go to next token.
            if (token[i].ty != TK_NUM) { // next token if + is recognized.
                error(i);
            }
            printf("  add rax, %d\n", tokens[i].val);
            i++;
            continue;
        }  // next tokens.

        if (tokens[i].ty == '-') {
            i++;  // go to next token.
            if (token[i].ty != TK_NUM) { // next token if - is recognized.
                error(i);
            }
            printf("  sub rax, %d\n", tokens[i].val);
            i++;
            continue;
        }  // next tokens.

        error(i);
    }

    printf("  ret\n");
    return 0;
}

