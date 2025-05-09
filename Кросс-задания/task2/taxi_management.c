#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

// Структура сообщения для очередей 
struct msgbuf {
    long mtype;         // Тип сообщения (PID для драйверов, 1 для CLI)
    char mtext[256];    // Содержимое сообщения
};

// Структура для хранения информации о драйверах в CLI
struct driver {
    pid_t pid;
    char status[50];    // "Available" или "Busy <remaining_time>"
};

// Глобальные переменные 
#define MAX_DRIVERS 100
struct driver drivers[MAX_DRIVERS];
int driver_count = 0;
int msgqid_to_drivers, msgqid_from_drivers;

// Поиск индекса драйвера по PID
int find_driver_index(pid_t pid) {
    for (int i = 0; i < driver_count; i++) {
        if (drivers[i].pid == pid) return i;
    }
    return -1;
}

// Обновление статуса драйвера в CLI
void update_driver_status(pid_t pid, const char *status) {
    int idx = find_driver_index(pid);
    if (idx != -1) {
        strcpy(drivers[idx].status, status);
    }
}

// CLI: Создание нового драйвера
void create_driver() {
    pid_t pid = fork();
    if (pid == 0) {
        // Дочерний процесс: запуск кода драйвера
        char *args[] = {"./taxi_management", "driver", NULL};
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        // Родительский процесс: сохранение информации о драйвере
        drivers[driver_count].pid = pid;
        strcpy(drivers[driver_count].status, "Available");
        driver_count++;
        printf("Создан драйвер с PID: %d\n", pid);
    } else {
        perror("fork failed");
    }
}

// CLI: Отправка задачи драйверу
void send_task(pid_t pid, int task_timer) {
    struct msgbuf msg;
    msg.mtype = pid;
    snprintf(msg.mtext, sizeof(msg.mtext), "TASK %d", task_timer);
    if (msgsnd(msgqid_to_drivers, &msg, strlen(msg.mtext) + 1, 0) == -1) {
        perror("msgsnd failed");
    }
}

// CLI: Запрос статуса драйвера
void get_status(pid_t pid) {
    struct msgbuf msg;
    msg.mtype = pid;
    strcpy(msg.mtext, "STATUS");
    if (msgsnd(msgqid_to_drivers, &msg, strlen(msg.mtext) + 1, 0) == -1) {
        perror("msgsnd failed");
    }
}

// CLI: Вывод списка всех драйверов
void get_drivers() {
    if (driver_count == 0) {
        printf("Нет активных драйверов.\n");
    } else {
        for (int i = 0; i < driver_count; i++) {
            if (strncmp(drivers[i].status, "Busy", 4) == 0) {
                printf("PID %d: Busy\n", drivers[i].pid);
            } else {
                printf("PID %d: %s\n", drivers[i].pid, drivers[i].status);
            }
        }
    }
}

// Драйвер: Обработка сигнала завершения задачи
volatile sig_atomic_t task_ended = 0;
void handle_alarm(int sig) {
    task_ended = 1;
}

// Основная функция драйвера
void driver_main() {
    key_t key_to = ftok(".", 'A');
    key_t key_from = ftok(".", 'B');
    msgqid_to_drivers = msgget(key_to, 0666);
    msgqid_from_drivers = msgget(key_from, 0666);
    pid_t my_pid = getpid();

    signal(SIGALRM, handle_alarm);

    char status[50] = "Available";
    time_t end_time = 0;
    struct msgbuf msg;

    while (1) {
        if (task_ended) {
            task_ended = 0;
            strcpy(status, "Available");
            struct msgbuf reply;
            reply.mtype = 1;
            snprintf(reply.mtext, sizeof(reply.mtext), "PID %d: Available", my_pid);
            msgsnd(msgqid_from_drivers, &reply, strlen(reply.mtext) + 1, 0);
            end_time = 0;
        }

        int ret = msgrcv(msgqid_to_drivers, &msg, sizeof(msg.mtext), my_pid, 0);
        if (ret == -1) {
            if (errno == EINTR) continue; 
            perror("msgrcv failed");
            exit(1);
        }

        if (strcmp(msg.mtext, "STATUS") == 0) {
            struct msgbuf reply;
            reply.mtype = 1;
            if (strcmp(status, "Available") == 0) {
                snprintf(reply.mtext, sizeof(reply.mtext), "PID %d: Available", my_pid);
            } else {
                int remaining = end_time - time(NULL);
                if (remaining < 0) remaining = 0;
                snprintf(reply.mtext, sizeof(reply.mtext), "PID %d: Busy %d", my_pid, remaining);
            }
            msgsnd(msgqid_from_drivers, &reply, strlen(reply.mtext) + 1, 0);
        } else if (strncmp(msg.mtext, "TASK", 4) == 0) {
            int task_timer;
            sscanf(msg.mtext, "TASK %d", &task_timer);
            struct msgbuf reply;
            reply.mtype = 1;
            if (strcmp(status, "Available") == 0) {
                strcpy(status, "Busy");
                end_time = time(NULL) + task_timer;
                alarm(task_timer);
                snprintf(reply.mtext, sizeof(reply.mtext), "PID %d: Busy %d", my_pid, task_timer);
            } else {
                int remaining = end_time - time(NULL);
                if (remaining < 0) remaining = 0;
                snprintf(reply.mtext, sizeof(reply.mtext), "PID %d: Busy %d", my_pid, remaining);
            }
            msgsnd(msgqid_from_drivers, &reply, strlen(reply.mtext) + 1, 0);
        }
    }
}

// Основная функция CLI
int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "driver") == 0) {
        driver_main();
        return 0;
    }

    // Инициализация очередей сообщений
    key_t key_to = ftok(".", 'A');
    key_t key_from = ftok(".", 'B');
    msgqid_to_drivers = msgget(key_to, IPC_CREAT | 0666);
    msgqid_from_drivers = msgget(key_from, IPC_CREAT | 0666);
    if (msgqid_to_drivers == -1 || msgqid_from_drivers == -1) {
        perror("msgget failed");
        exit(1);
    }

    char input[256];
    fd_set readfds;
    printf("Введите команду (create_driver, send_task <pid> <timer>, get_status <pid>, get_drivers):\n");

    while (1) {
        

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        struct timeval tv = {0, 100000}; //100 мс   

        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            if (strcmp(input, "create_driver") == 0) {
                create_driver();
            } else if (strncmp(input, "send_task", 9) == 0) {
                pid_t pid;
                int task_timer;
                if (sscanf(input, "send_task %d %d", &pid, &task_timer) == 2) {
                    send_task(pid, task_timer);
                } else {
                    printf("Использование: send_task <pid> <task_timer>\n");
                }
            } else if (strncmp(input, "get_status", 10) == 0) {
                pid_t pid;
                if (sscanf(input, "get_status %d", &pid) == 1) {
                    get_status(pid);
                } else {
                    printf("Использование: get_status <pid>\n");
                }
            } else if (strcmp(input, "get_drivers") == 0) {
                get_drivers();
            } else {
                printf("Неизвестная команда\n");
            }
        }

        // Получение сообщений от драйверов
        struct msgbuf msg;
        ssize_t len = msgrcv(msgqid_from_drivers, &msg, sizeof(msg.mtext), 1, IPC_NOWAIT);
        if (len > 0) {
            pid_t pid;
            char status[50];
            if (sscanf(msg.mtext, "PID %d: %49[^\n]", &pid, status) == 2) {
                update_driver_status(pid, status);
                printf("%s\n", msg.mtext);
            }
        }
    }

    // Очистка (не достигается в этом цикле)
    msgctl(msgqid_to_drivers, IPC_RMID, NULL);
    msgctl(msgqid_from_drivers, IPC_RMID, NULL);
    return 0;
}