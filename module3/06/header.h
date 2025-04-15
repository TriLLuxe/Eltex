#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_KEY 1234
#define MAX_TEXT 512
#define MTYPE_1 1
#define MTYPE_2 2
#define MTYPE_EXIT 10
struct msgbuf {
    long mtype;
    char mtext[MAX_TEXT];
};
/* Функция обмена сообщениями */
void chat(int msgid, int sender_type, int peer_type);
#endif