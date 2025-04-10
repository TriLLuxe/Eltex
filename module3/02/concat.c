#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <строка1> <строка2> ...\n", argv[0]);
        return 1;
    }
    printf("Результат = ");
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);
    }
    printf("\n");
    return 0;
}