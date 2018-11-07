#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Invalid num of args.\n");
		return 1;
	}

    char *p = argv[1];


	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
    // アドレス参照子(&)
	printf("  mov rax, %ld\n", strtol(p, &p, 10));  // strtolは、(対象文字列, 変換できなかった残りの文字列を格納するアドレス, 基数)なので、10+4なら、10が戻り値、+4がポインタ先に格納となる。

    // while(var) でvarが0でない場合は、True.
    while (*p) {
        if (*p == '+') {  // 間接参照演算子(*)
            p++;
            printf("   add rax, %ld\n", strtol(p, &p, 10));
            continue;
        }
        if (*p == '-') {
            p++;
            printf("   sub rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        fprintf(stderr, "Unexpected Input String :'%c'\n", *p);
        return 1;
    }

	printf("  ret\n");
	return 0;
}

