#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>

void sem_lock(int sem_id);
void sem_unlock(int sem_id);

#endif 