#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>
#define MAX_NUMBERS 10

// Структура для разделяемой памяти
typedef struct {
    int numbers[MAX_NUMBERS];
    int size;
    int min;
    int max;
    int count; // Счетчик обработанных наборов
} SharedData;

//  для обработки сигнала
volatile sig_atomic_t running = 1;
SharedData *shared_data = NULL;
int shm_id = -1;
int sem_id = -1;

void signal_handler(int sig) {
    running = 0;
}
void clean() {
    if (shared_data != NULL) {
        shmdt(shared_data);
    }
    if (shm_id != -1) {
        shmctl(shm_id, IPC_RMID, NULL);
    }
    if (sem_id != -1) {
        semctl(sem_id, 0, IPC_RMID);
    }
}

int main(int argc, char *argv[]) {
    // Настройка обработчика сигнала SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Ошибка настройки обработчика SIGINT");
        return 1;
    }
    key_t shm_key = ftok("semaphore.c", 'a');
    if (shm_key == -1) {
        perror("Ошибка создания ключа для разделяемой памяти");
        clean();
        exit(1);
    }
    shm_id = shmget(shm_key, sizeof(SharedData), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Ошибка создания разделяемой памяти");
        clean();
        exit(1);
    }
    shared_data = shmat(shm_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("Ошибка присоединения разделяемой памяти");
        clean();
        exit(1);
    }  
    shared_data->size = 0;
    shared_data->min = 0;
    shared_data->max = 0;
    shared_data->count = 0;
    key_t sem_key = ftok("semaphore.c", 'b');
    if (sem_key == -1) {
        perror("Ошибка создания ключа для семафора");
        clean();
        exit(1);
    }
    sem_id = semget(sem_key, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Ошибка создания семафора");
        clean();
        exit(1);
    }
    if(semctl(sem_id, 0, SETVAL, 0)==-1 || semctl(sem_id, 1, SETVAL, 0)==-1)
    {
        perror("Ошибка инициализации семафора");
        clean();
        exit(1);
    } 
    srand(time(NULL));
    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("Ошибка создания процесса");
            clean();
            exit(1);
        case 0: // Дочерний процесс
            while(running) {
                sem_lock(sem_id,0);// Ожидание от родителя
                //Чтение набора чисел
                int min = shared_data->numbers[0];
                int max = shared_data->numbers[0];
                for (int i = 1; i<shared_data->size;++i){
                    if (shared_data->numbers[i] < min) min = shared_data->numbers[i];
                    if (shared_data->numbers[i] > max) max = shared_data->numbers[i];
                }

                //Запись в разделяемую память
                shared_data->min = min;
                shared_data->max = max;
                //sleep(1);
                sem_unlock(sem_id,1); //Сигнализируем родителю о завершении обработки
            }
            shmdt(shared_data);
            exit(0);

        default:
            while (running) {
            // Родительский процесс
            
            // Генерация случайных чисел
            shared_data->size = rand() % MAX_NUMBERS + 1;
            for (int i = 0; i < shared_data->size; ++i) {
                shared_data->numbers[i] = rand() % 100;
            }
            sem_unlock(sem_id,0);// Сигнал дочернему процессу о готовности данных
            
            // Ждем на чтение 
            sem_lock(sem_id,1);
            //Вывод результатов
            printf("Набор %d: %d чисел, Минимальное: %d, Максимальное: %d\n",
                shared_data->count + 1, shared_data->size, shared_data->min, shared_data->max);
            for(int i = 0; i < shared_data->size; ++i) {
                printf("%d ", shared_data->numbers[i]);
            }
            printf("\n");
            shared_data->count++;
            
}
// Завершение работы
printf("Получен SIGINT. Обработано наборов: %d\n", shared_data->count);
sem_unlock(sem_id, 0); 
// Ожидание завершения дочернего процесса
wait(NULL);
clean();
return 0;
}
}