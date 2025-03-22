#include "header.h"
#include <stdio.h>

int main() {
    int choice;
    int count;
    double numbers[10];

    while (1) {
        printf("\nКалькулятор\n");
        printf("1. Сложение\n");
        printf("2. Вычитание\n");
        printf("3. Умножение\n");
        printf("4. Деление\n");
        printf("5. Выход\n");
        printf("Выберите операцию (1-5): ");
        scanf("%d", &choice);

        if (choice == 5) {
            break;
        }

        if (choice < 1 || choice > 5) {
            printf("Ошибка: выберите число от 1 до 5\n");
            continue;
        }

        printf("Введите количество чисел (1-100): ");
        scanf("%d", &count);
        if (count < 1 || count > 10) {
            printf("Ошибка: количество чисел должно быть от 1 до 10\n");
            continue;
        }

        printf("Введите %d чисел: ", count);
        for (int i = 0; i < count; i++) {
            scanf("%lf", &numbers[i]);
        }

        double result;
        switch (choice) {
            case 1:
                result = add(count, numbers[0], numbers[1], numbers[2], numbers[3], numbers[4],
                    numbers[5], numbers[6], numbers[7], numbers[8], numbers[9]);
                printf("Результат: %lf\n", result);
                break;
            case 2:
                result = subtract(count, numbers[0], numbers[1], numbers[2], numbers[3], numbers[4],
                    numbers[5], numbers[6], numbers[7], numbers[8], numbers[9]);
                printf("Результат: %lf\n", result);
                break;
            case 3:
                result = multiply(count, numbers[0], numbers[1], numbers[2], numbers[3], numbers[4],
                    numbers[5], numbers[6], numbers[7], numbers[8], numbers[9]);
                printf("Результат: %lf\n", result);
                break;
            case 4:
                result = divide(count, numbers[0], numbers[1], numbers[2], numbers[3], numbers[4],
                    numbers[5], numbers[6], numbers[7], numbers[8], numbers[9]);
                printf("Результат: %lf\n", result);
                break;
            default:
                printf("Неизвестная операция\n");
                break;
        }
    }
    return 0;
}