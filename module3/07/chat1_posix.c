#include "header.h"

int main() {
    /* Настройка атрибутов очереди */
    struct mq_attr attr = {
        .mq_msgsize = MAX_TEXT,
    };

    /* Создание или открытие очереди */
    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    chat(mq, PRIORITY_1, PRIORITY_2);

    /* Закрытие и удаление очереди */
    if (mq_close(mq) == -1) {
        perror("mq_close");
        exit(1);
    }
    if (mq_unlink(QUEUE_NAME) == -1) {
        perror("mq_unlink");
        exit(1);
    }

    printf("Чат завершен. Очередь удалена.\n");
    return 0;
}