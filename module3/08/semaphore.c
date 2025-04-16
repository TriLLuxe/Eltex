#include "semaphore.h"
#include <stdio.h>
#include <stdlib.h>

void sem_lock(int sem_id) {
    struct sembuf op = {0, -1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("Ошибка блокировки семафора");
        exit(1);
    }
}

void sem_unlock(int sem_id) {
    struct sembuf op = {0, 1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("Ошибка разблокировки семафора");
        exit(1);
    }
}