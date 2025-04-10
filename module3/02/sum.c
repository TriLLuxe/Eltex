#include <stdio.h>
#include <stdlib.h>
int main (int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <число1> <число2> ...\n", argv[0]);
        return 1;
    }
    
    double sum = 0;
    for (int i = 1; i < argc; i++) {
        sum += atof(argv[i]);
    }
    
    printf("Сумма: %.3f\n", sum);
    return 0;
}