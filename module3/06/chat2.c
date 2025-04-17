#include "header.h"

int main() {
    /* Подключение к существующей очереди */
    key_t key = ftok(".", 'a');
    int msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    chat(msgid, MTYPE_2, MTYPE_1); /* Отправка с mtype=2, получение с mtype=1 */
    sleep(1); 
    printf("Чат завершен.\n");
    return 0;
}