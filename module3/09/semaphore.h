#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>

void sem_lock(int sem_id, int sem_num);
void sem_unlock(int sem_id, int sem_num);   
#endif 