#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "header.h"


int main (int argc, char *argv[]) {

    if (argc < 2){
        fprintf(stderr, "Использование: %s <сторона1> <сторона2> ...\n", argv[0]);
        return 1;
    }
    int num_sides = argc - 1;
    double *sides = malloc(num_sides * sizeof(double));
    double *results = malloc(num_sides * sizeof(double));
    if (!sides || !results) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        free(sides);
        free(results);
        return 1;
    }

    for (int i = 0; i < num_sides; i++) {
        sides[i] = atof(argv[i + 1]);
        if (sides[i] <= 0) {
            fprintf(stderr, "Ошибка: все стороны должны быть положительными числами\n");
            free(sides);
            free(results);
            return 1;
        }
    }
    int half = num_sides / 2;
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Ошибка создания процесса\n");
        free(sides);
        free(results);
        return 1;
    } 
    else if (pid == 0) {
        process_squares(sides, 0, half, results);
        printf("Площади квадратов:\n 0 - Главный процесс, 1 - Дочерний процесс\n");
        for (int i = 0; i < half; i++) {
            printf("Сторона = %.3f, площадь = %.3f (вычислено процессом %d)\n",
                       sides[i], results[i], 1); // 1 - Дочерний процесс, 0 - Главный процесс
        }
        printf("\n");
        free(sides);
        free(results);
        exit(0);
    } 
    else {
        process_squares(sides, half, num_sides, results);
        wait(NULL);
        for (int i = half; i < num_sides; i++) {
            printf("Сторона = %.3f, площадь = %.3f (вычислено процессом %d)\n",
                       sides[i], results[i], 0); // 1 - Дочерний процесс, 0 - Главный процесс
        }
        
    }
    
    free(sides);
    free(results);
    return 0;
}