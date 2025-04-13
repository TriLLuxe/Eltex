#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#define FILENAME "random_numbers.txt"

volatile sig_atomic_t file_locked = 1;

void handle_sigusr1(int sig) {
    file_locked = 1;
}

void handle_sigusr2(int sig) {
    file_locked = 0;
}

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
    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("Ошибка создания процесса");
            return 1;
        case 0: // Дочерний процесс
            close(fd[0]);
            signal(SIGUSR1, handle_sigusr1);
            signal(SIGUSR2, handle_sigusr2);
            srand(time(NULL));
            for (int i = 0; i < num_numbers; i++) {
                printf("Дочерний процесс итерации %d\n", i);
                // Генерируем и отправляем число
                int random_number = rand() % 100;
                printf("Дочерний процесс генерирует: %d\n", random_number);
                if (write(fd[1], &random_number, sizeof(random_number)) == -1) {
                    perror("Ошибка записи в канал");
                    close(fd[1]);
                    exit(1);
                }
                // Ждем SIGUSR2 перед чтением файла
                while (file_locked) {
                    printf("Дочерний процесс ждет разрешения на чтение файла...\n");
                    pause(); // Блокируем до получения сигнала
                }
                // Читаем файл
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
               file_locked = 1; 
            }
            close(fd[1]);
            exit(0);
        default: // Родительский процесс
            close(fd[1]);
            signal(SIGUSR1, SIG_IGN);
            signal(SIGUSR2, SIG_IGN);
            FILE *file = fopen(FILENAME, "w");
            if (!file) {
                perror("Ошибка открытия файла");
                close(fd[0]);
                return 1;
            }
            fclose(file); 
            int random_number;
            for (int i = 0; i < num_numbers; i++) {
                
                printf("Родитель итерации %d\n", i);
                printf("Родитель ждет число от дочернего процесса...\n");
                if (read(fd[0], &random_number, sizeof(random_number)) == -1) {
                    perror("Ошибка чтения из канала");
                    close(fd[0]);
                    return 1;
                }
                // Блокируем доступ к файлу
                printf("Родитель отправляет SIGUSR1\n");
                kill(pid, SIGUSR1);
                // Записываем число в файл
                file = fopen(FILENAME, "a");
                if (!file) {
                    perror("Ошибка открытия файла");
                    close(fd[0]);
                    return 1;
                }
                printf("Родитель записывает: %d\n", random_number);
                fprintf(file, "%d\n", random_number);
                fclose(file);
                // Разрешаем дочернему процессу читать
                printf("Родитель отправляет SIGUSR2\n");
                kill(pid, SIGUSR2);
            }
            close(fd[0]);
            wait(NULL);
    }
    return 0;
}