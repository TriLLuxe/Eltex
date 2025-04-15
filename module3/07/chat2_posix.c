#include "header.h"

int main() {
    /* Открытие очереди */
    mqd_t mq = mq_open(QUEUE_NAME, O_RDWR);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    chat(mq, PRIORITY_2, PRIORITY_1);

    /* Закрытие очереди */
    if (mq_close(mq) == -1) {
        perror("mq_close");
        exit(1);
    }

    printf("Чат завершен.\n");
    return 0;
}