#include "header.h"

int main() {
    key_t key = ftok(".", 'a');
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    chat(msgid, MTYPE_1, MTYPE_2);
    sleep(1);
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
    printf("Чат завершен. Очередь удалена.\n");
    return 0;
}