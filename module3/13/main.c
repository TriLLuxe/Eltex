#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_NUMBERS 10
#define SHM_NAME "/myshm"
#define SEM_PARENT "/sem_parent"
#define SEM_CHILD "/sem_child"


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
sem_t *sem_parent = NULL;
sem_t *sem_child = NULL;
int shm_fd = -1;

void signal_handler(int sig) {
    running = 0;
}
void clean() {
    if (shared_data != NULL) {
        munmap(shared_data, sizeof(SharedData));
    }
    if (shm_fd != -1) {
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }
    if (sem_parent != NULL) {
        sem_close(sem_parent);
        sem_unlink(SEM_PARENT);
    }
    if (sem_child != NULL) {
        sem_close(sem_child);
        sem_unlink(SEM_CHILD);
    }
}

int main(int argc, char *argv[]) {
    // Настройка обработчика сигнала SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Ошибка настройки обработчика SIGINT");
        return 1;
    }
    
    
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка создания разделяемой памяти");
        clean();
        exit(1);
    }
    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("Ошибка установки размера разделяемой памяти");
        clean();
        exit(1);
    }
    shared_data = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("Ошибка отображения разделяемой памяти");
        clean();
        exit(1);
    }
    // Инициализация sharedData
    shared_data->size = 0;
    shared_data->min = 0;
    shared_data->max = 0;
    shared_data->count = 0;
    
    //Создание семафоров
    sem_parent = sem_open(SEM_PARENT, O_CREAT, 0644, 0);
    if (sem_parent == SEM_FAILED) {
        perror("Ошибка создания семафора родителя");
        clean();
        exit(1);
    }
    sem_child = sem_open(SEM_CHILD, O_CREAT, 0644, 0);
    if (sem_child == SEM_FAILED) {
        perror("Ошибка создания семафора дочернего процесса");
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
                sem_wait(sem_parent);// Ожидание от родителя
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
                sem_post(sem_child); //Сигнализируем родителю о завершении обработки
            }
            munmap(shared_data, sizeof(SharedData)); 
            exit(0);

        default:
            while (running) {
            // Родительский процесс
            
            // Генерация случайных чисел
            shared_data->size = rand() % MAX_NUMBERS + 1;
            for (int i = 0; i < shared_data->size; ++i) {
                shared_data->numbers[i] = rand() % 100;
            }
            sem_post(sem_parent);// Сигнал дочернему процессу о готовности данных
            
            // Ждем на чтение 
            sem_wait(sem_child);
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
sem_post(sem_parent); 
// Ожидание завершения дочернего процесса
wait(NULL);
clean();
return 0;
}
}
