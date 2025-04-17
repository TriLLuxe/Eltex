#include "semaphore.h"
#include <stdio.h>
#include <stdlib.h>

void sem_lock(int sem_id, int sem_num) {    
    struct sembuf op = {sem_num, -1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("Ошибка блокировки семафора");
        exit(1);
    }
}

void sem_increase(int sem_id, int sem_num) {
    struct sembuf op = {sem_num, 1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("Ошибка разблокировки семафора");
        exit(1);
    }
}
void sem_unlock(int sem_id, int sem_num) {
    struct sembuf op[2]= {{sem_num, 0, 0}, {sem_num, 1, 0}};
    if (semop(sem_id, op, 2) == -1) {
        perror("Ошибка увеличения семафора");
        exit(1);
    }
}