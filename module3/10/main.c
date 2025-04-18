#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>
#define FILENAME "random_numbers.txt"


int main(int argc, char *argv[]) {
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

    // Создание семафора POSIX
    sem_t *wrt_sem = sem_open("/wrt_sem", O_CREAT | O_EXCL, 0644, 1);

    if (wrt_sem == SEM_FAILED) {
        perror("Ошибка создания семафора");
        return 1;
    }


    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("Ошибка создания процесса");
            return 1;
        case 0: // Дочерний процесс
            close(fd[0]);
            srand(time(NULL));
            for (int i = 0; i < num_numbers; i++) {
                sem_wait(wrt_sem); // Блокируем писателя
                FILE *file = fopen(FILENAME, "r");
                if (file) {
                    int number;
                    printf("Дочерний процесс читает файл:\n");
                    while (fscanf(file, "%d", &number) == 1) {
                        printf("%d\n", number);
                    }
                    fclose(file);
                } else {
                    perror("Ошибка открытия файла в дочернем процессе");
                }
                sem_post(wrt_sem); // Разблокируем писателя

                int random_number = rand() % 100;
                if (write(fd[1], &random_number, sizeof(random_number)) == -1) {
                    perror("Ошибка записи в канал");
                    close(fd[1]);
                    exit(1);
                }
            }
            close(fd[1]);
            exit(0);
        default: // Родительский процесс
            close(fd[1]);
            FILE *file = fopen(FILENAME, "w");
            if (!file) {
                perror("Ошибка открытия файла");
                close(fd[0]);
                return 1;
            }
            fclose(file);

            int random_number;
            for (int i = 0; i < num_numbers; i++) {
                if (read(fd[0], &random_number, sizeof(random_number)) == -1) {
                    perror("Ошибка чтения из канала");
                    close(fd[0]);
                    return 1;
                }
                sem_wait(wrt_sem); // Блокируем писателя
                file = fopen(FILENAME, "a");
                if (!file) {
                    perror("Ошибка открытия файла");
                    close(fd[0]);
                    return 1;
                }
                printf("Родитель записывает: %d\n", random_number);
                fprintf(file, "%d\n", random_number);
                fclose(file);
                sem_post(wrt_sem); // Разблокируем писателя
            }
            close(fd[0]);
            wait(NULL);

            sem_close(wrt_sem); // Закрываем семафор
            sem_unlink("/wrt_sem"); // Удаляем семафор
    }
    return 0;
}