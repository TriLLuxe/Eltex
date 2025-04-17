#include "semaphore.h"
#define FILENAME "random_numbers.txt"
#define NUM_READERS 3

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
    key_t key = ftok(".",'a');
    int sem_id = semget(key, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Ошибка создания семафора");
        return 1;
    }
    //0-й семафор для писателя, 1-й семафор для активных читателей
    if (semctl(sem_id, 0, SETVAL, 1) == -1 || semctl(sem_id, 1, SETVAL, 0) == -1) {
        perror("Ошибка инициализации семафора");
        return 1;
    }

    // Создание файла для очистки
    FILE *file = fopen(FILENAME, "w");
    if (!file) {
        perror("Ошибка открытия файла");
        return 1;
    }
    fclose(file);
    // Распределение чисел между читателями
    int base = num_numbers / NUM_READERS;
    int remainder = num_numbers % NUM_READERS;

    // Создание читателей
    pid_t pids[NUM_READERS];
    for (int i = 0; i < NUM_READERS; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("Ошибка создания процесса");
            return 1;
        }
        if (pids[i] == 0) { // Дочерний процесс (читатель)
            close(fd[0]);
            srand(time(NULL)*getpid());
            // Количество чисел для текущего читателя (сколько чисел он будет генерировать)
            int numbers_to_generate = base + (i < remainder ? 1 : 0);
            for (int j = 0; j < num_numbers; j++) {
                
                sem_increase(sem_id,1); //Увеличение активных читателей
                if (semctl(sem_id, 1, GETVAL) == 1) {
                    sem_lock(sem_id, 0); //  Первый читатель блокирует запись
                }
                // Чтение файла
                file = fopen(FILENAME, "r");
                if (file) {
                    int number;
                    printf("Читатель %d читает файл:\n", i);
                    while (fscanf(file, "%d", &number) == 1) {
                        printf("Читатель %d: %d\n", i, number);
                    }
                    fclose(file);
                } else {
                    perror("Ошибка открытия файла в читателе");
                }
                sem_lock(sem_id,1);
                if (semctl(sem_id, 1, GETVAL) == 0) {
                    sem_unlock(sem_id, 0); // Разблокировка писателя
                }

                // Отправка числа писателю
                if (j < numbers_to_generate) {
                    int random_number = rand() % 100;
                    if (write(fd[1], &random_number, sizeof(random_number)) == -1) {
                        perror("Ошибка записи в канал");
                        exit(1);
                    }
                    printf("Читатель %d отправил число: %d\n", i, random_number);
                    sleep(1); 
                }
            
            }
            close(fd[1]);
            exit(0);
        }
    }

    // Родительский процесс (писатель)
    close(fd[1]);
    int random_number;
    for (int i = 0; i < num_numbers ; i++) {
        if (read(fd[0], &random_number, sizeof(random_number)) == -1) {
            perror("Ошибка чтения из канала");
            break;
        }
        
        sem_lock(sem_id,0);
        file = fopen(FILENAME, "a");
        if (file) {
            printf("Писатель записывает: %d\n", random_number);
            fprintf(file, "%d\n", random_number);
            fclose(file);
        }
        sem_unlock(sem_id,0);
        
    }
    close(fd[0]);

    // Ожидание завершения читателей
    for (int i = 0; i < NUM_READERS; i++) {
        wait(NULL);
    }

    // Удаление семафоров
    semctl(sem_id, 0, IPC_RMID);
    return 0;
}