#include "header.h"

/* Функция обмена сообщениями */
void chat(mqd_t mq, int my_priority, int peer_priority) {
    char buffer[MAX_TEXT];
    unsigned int priority;

    // chat1 (PRIORITY_1) начинает с получения, chat2 (PRIORITY_2) — с отправки 
    int is_initiator = (my_priority == PRIORITY_1);

    while (1) {
        if (is_initiator) {
            /* Получение сообщения от chat2 */
            ssize_t bytes_read = mq_receive(mq, buffer, MAX_TEXT, &priority);
            if (bytes_read == -1) {
                perror("mq_receive");
                exit(1);
            }
            buffer[bytes_read] = '\0'; 
            printf("Получено: %s\n", buffer);

            /* Проверка завершения */
            if ((strcmp(buffer, "exit") == 0 || strcmp(buffer, "q") == 0) && priority == PRIORITY_EXIT) {
                break;
            }
        }

        /* Отправка сообщения */
        printf("Введите сообщение: ");
        if (fgets(buffer, MAX_TEXT, stdin) == NULL) {
            perror("fgets");
            exit(1);
        }
        buffer[strcspn(buffer, "\n")] = '\0'; 

        //PRIORITY_EXIT для "exit" или "q", иначе my_priority 
        int send_priority = (strcmp(buffer, "exit") == 0 || strcmp(buffer, "q") == 0) ? PRIORITY_EXIT : my_priority;
        if (mq_send(mq, buffer, strlen(buffer) + 1, send_priority) == -1) {
            perror("mq_send");
            exit(1);
        }

        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "q") == 0) {
            break;
        }

        if (!is_initiator) {
            /* Получение ответа от chat1 */
            ssize_t bytes_read = mq_receive(mq, buffer, MAX_TEXT, &priority);
            if (bytes_read == -1) {
                perror("mq_receive");
                exit(1);
            }
            buffer[bytes_read] = 
            printf("Получено: %s\n", buffer);

            /* Проверка завершения */
            if ((strcmp(buffer, "exit") == 0 || strcmp(buffer, "q") == 0) && priority == PRIORITY_EXIT) {
                break;
            }
        }
    }
}