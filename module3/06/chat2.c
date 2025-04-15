#include "header.h"

int main() {
    /* Подключение к существующей очереди */
    int msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    chat(msgid, 2, 1); /* Отправка с mtype=2, получение с mtype=1 */

    printf("Чат завершен.\n");
    return 0;
}