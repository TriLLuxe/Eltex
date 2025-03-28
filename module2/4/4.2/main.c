#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    srand(time(NULL)); // Инициализация генератора случайных чисел
    PriorityQueue* pq = createPriorityQueue();
    int choice, priority, count;
    char message[50];

    while (1) {
        printf("\nМеню:\n");
        printf("1. Добавить элемент\n");
        printf("2. Извлечь первый элемент\n");
        printf("3. Извлечь элемент с указанным приоритетом\n");
        printf("4. Извлечь элемент с приоритетом не ниже заданного\n");
        printf("5. Показать очередь\n");
        printf("6. Случайное заполнение\n");
        printf("7. Выход\n");
        printf("Выберите действие (1-7): ");
        scanf("%d", &choice);
        clearInputBuffer(); // Очистка буфера ввода

        switch (choice) {
            case 1: // Добавить элемент
                printf("Введите приоритет (0-255): ");
                scanf("%d", &priority);
                if (priority < 0 || priority > 255) {
                    printf("Ошибка: приоритет должен быть от 0 до 255!\n");
                    break;
                }
                clearInputBuffer();
                printf("Введите сообщение: ");
                fgets(message, sizeof(message), stdin);
                message[strcspn(message, "\n")] = 0;
                char* data = strdup(message);
                enqueue(pq, priority, data);
                printf("Элемент добавлен.\n");
                printQueue(pq);
                break;

            case 2: // Извлечь первый элемент
                {
                    char* result = (char*)dequeue(pq);
                    if (result) {
                        printf("Извлечен элемент: %s\n", result);
                        free(result);
                    } else {
                        printf("Очередь пуста.\n");
                    }
                    printQueue(pq);
                }
                break;

            case 3: // Извлечь элемент с указанным приоритетом
                printf("Введите приоритет для извлечения (0-255): ");
                scanf("%d", &priority);
                if (priority < 0 || priority > 255) {
                    printf("Ошибка: приоритет должен быть от 0 до 255!\n");
                    break;
                }
                {
                    char* result = (char*)dequeueWithPriority(pq, priority);
                    if (result) {
                        printf("Извлечен элемент: %s\n", result);
                        free(result);
                    } else {
                        printf("Элемент с приоритетом %d не найден.\n", priority);
                    }
                    printQueue(pq);
                }
                break;

            case 4: // Извлечь элемент с приоритетом не ниже заданного
                printf("Введите минимальный приоритет (0-255): ");
                scanf("%d", &priority);
                if (priority < 0 || priority > 255) {
                    printf("Ошибка: приоритет должен быть от 0 до 255!\n");
                    break;
                }
                {
                    char* result = (char*)dequeueWithMinPriority(pq, priority);
                    if (result) {
                        printf("Извлечен элемент: %s\n", result);
                        free(result);
                    } else {
                        printf("Элемент с приоритетом не выше %d не найден.\n", priority);
                    }
                    printQueue(pq);
                }
                break;

            case 5: // Показать очередь
                printQueue(pq);
                break;

            case 6: // Случайное заполнение
                printf("Введите количество элементов для добавления: ");
                scanf("%d", &count);
                if (count < 0) {
                    printf("Ошибка: количество должно быть положительным!\n");
                    break;
                }
                for (int i = 0; i < count; i++) {
                    priority = rand() % 256; // Случайный приоритет от 0 до 255
                    sprintf(message, "%d", i + 1);
                    char* data = strdup(message);
                    printf("Добавляем: [%d: %s]\n", priority, data);
                    enqueue(pq, priority, data);
                    printQueue(pq);
                }
                printf("Случайное заполнение завершено.\n");
                break;

            case 7: // Выход
                while (pq->head != NULL) {
                    Node* temp = pq->head;
                    pq->head = pq->head->next;
                    free(temp->data);
                    free(temp);
                }
                free(pq);
                printf("Программа завершена.\n");
                return 0;

            default:
                printf("Неверный выбор. Попробуйте снова.\n");
                break;
        }
    }

    return 0;
}