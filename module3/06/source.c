#include "header.h"
#include <errno.h>

/* Функция обмена сообщениями */
void chat(int msgid, int my_type, int peer_type) {
    struct msgbuf msg;
    int is_initiator = (my_type == MTYPE_1);

    while (1) {
        if (is_initiator) {
            /* Получение сообщения от chat2 */
            if (msgrcv(msgid, &msg, sizeof(msg.mtext), 0, 0) == -1) {
                if (errno == EIDRM) {
                    // Очередь удалена
                    break;
                }
                perror("msgrcv");
                exit(1);
            }
            // Проверяем тип сообщения
            if (msg.mtype == MTYPE_EXIT) {
                if (strcmp(msg.mtext, "exit") == 0 || strcmp(msg.mtext, "q") == 0) {
                    break;
                }
            } else if (msg.mtype == peer_type) {
                printf("Получено: %s\n", msg.mtext);
            } else {
                continue;
            }
        }

        /* Отправка сообщения */
        printf("Введите сообщение: ");
        if (fgets(msg.mtext, MAX_TEXT, stdin) == NULL) {
            perror("fgets");
            exit(1);
        }
        msg.mtext[strcspn(msg.mtext, "\n")] = '\0'; 

        /* MTYPE_EXIT для "exit" или "q", иначе my_type */
        msg.mtype = (strcmp(msg.mtext, "exit") == 0 || strcmp(msg.mtext, "q") == 0) ? MTYPE_EXIT : my_type;
        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
            if (errno == EIDRM) {
                // Очередь удалена
                break;
            }
            perror("msgsnd");
            exit(1);
        }

        if (msg.mtype == MTYPE_EXIT) {
            break;
        }

        if (!is_initiator) {
            /* Получение ответа от chat1 */
            if (msgrcv(msgid, &msg, sizeof(msg.mtext), 0, 0) == -1) {
                if (errno == EIDRM) {
                    // Очередь удалена
                    break;
                }
                perror("msgrcv");
                exit(1);
            }
            /* Проверяем тип сообщения */
            if (msg.mtype == MTYPE_EXIT) {
                if (strcmp(msg.mtext, "exit") == 0 || strcmp(msg.mtext, "q") == 0) {
                    break;
                }
            } else if (msg.mtype == peer_type) {
                printf("Получено: %s\n", msg.mtext);
            } else {
                
                continue;
            }
        }
    }
}