#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <errno.h>

#define QUEUE_NAME "/mychat"
#define MAX_TEXT 512
#define PRIORITY_1 1
#define PRIORITY_2 2
#define PRIORITY_EXIT 10

/* Функция обмена сообщениями */
void chat(mqd_t mq, int my_priority, int peer_priority);

#endif