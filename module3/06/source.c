#include "header.h"
/* Функция обмена сообщениями */
void chat(int msgid, int my_type, int peer_type) {
    struct msgbuf msg;
    while (1) {
        //Отправка сообщения
        printf("Введите сообщение: ");
        fgets(msg.mtext, MAX_TEXT, stdin);
        msg.mtext[strcspn(msg.mtext, "\n")] = 0; 
        msg.mtype = my_type;
        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        if (strcmp(msg.mtext, "exit") == 0 || strcmp(msg.mtext, "q") == 0) {
            break;
        }

        /* Получение ответа от собеседника */
        if (msgrcv(msgid, &msg, sizeof(msg.mtext), peer_type, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Получено: %s\n", msg.mtext);

        if (strcmp(msg.mtext, "exit") == 0 || strcmp(msg.mtext, "q") == 0) {
            break;
        }
    }
}