#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#define FILENAME "random_numbers.txt"

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <количество случайных чисел>\n", argv[0]);
        return 1;
    }
    int num_numbers = atoi(argv[1]);
    if (num_numbers <= 0) {
        fprintf(stderr, "Количество случайных чисел должно быть положительным\n");
        return 1;
    }
    int fd[2];
    if (pipe(fd) == -1) {
        perror("Ошибка создания канала");
        return 1;
    }
    pid_t pid = fork();
    switch (pid) {
    
        case -1:
            perror("Ошибка создания процесса");
            return 1;
        case 0:
            close(fd[0]);
            srand(time(NULL));
            for (int i = 0; i < num_numbers; i++) {
                int random_number = rand() % 100;
                if (write(fd[1], &random_number, sizeof(random_number)) == -1) {
                    perror("Ошибка записи в канал");
                    close(fd[1]);
                    return 1;
                }
            }
            close(fd[1]);
            exit(0);
            
        default:
            close(fd[1]);
            FILE *file = fopen(FILENAME, "w");
            if (!file) {
                perror("Ошибка открытия файла");
                close(fd[0]);
                return 1;
            }
            int random_number;
            for (int i = 0; i < num_numbers; i++) {
                if (read(fd[0], &random_number, sizeof(random_number)) == -1) {
                    perror("Ошибка чтения из канала");
                    fclose(file);
                    close(fd[0]);
                    return 1;
                }
                printf("Получено: %d\n", random_number);
                fprintf(file, "%d\n", random_number);
            }
            fclose(file);
            close(fd[0]);
            wait(NULL);
    }
    return 0;
}