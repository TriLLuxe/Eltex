#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
int main(int argc, char *argv[]) {
    // Проверка количества аргументов
    if (argc != 4) {
        fprintf(stderr, "Использование: %s <IP_шлюза> <маска_подсети> <N>\n", argv[0]);
        return 1;
    }

    const char *gateway_ip_str = argv[1];
    const char *subnet_mask_str = argv[2];

    // Проверка и преобразование N в целое число
    int N;
    char extra;
    int result = sscanf(argv[3], "%d%c", &N, &extra);
    if (result != 1) {
        fprintf(stderr, "N должно быть целым числом: %s\n", argv[3]);
        return 1;
    }
    if (N <= 0) {
        fprintf(stderr, "N должно быть положительным числом\n");
        return 1;
    }

    // Преобразование IP-адреса шлюза и маски подсети в 32-битные числа
    uint32_t gateway_ip = ip_to_int(gateway_ip_str);
    uint32_t subnet_mask = ip_to_int(subnet_mask_str);

    // Вычисление адреса сети
    uint32_t network_address = gateway_ip & subnet_mask;

    // Инициализация генератора случайных чисел
    srand(time(NULL));

    int same_subnet_count = 0;

    // Генерация N случайных IP-адресов и подсчет совпадений с подсетью
    for (int i = 0; i < N; i++) {
        uint32_t random_ip = generate_random_ip();
        uint32_t random_network = random_ip & subnet_mask;
        if (random_network == network_address) {
            same_subnet_count++;
        }
    }

    // Вычисление процентов
    double same_subnet_percent = (double)same_subnet_count / N * 100;
    double other_subnet_percent = (double)(N - same_subnet_count) / N * 100;

    // Вывод результатов
    printf("Пакеты для своей подсети: %d (%.2f%%)\n", same_subnet_count, same_subnet_percent);
    printf("Пакеты для других сетей: %d (%.2f%%)\n", N - same_subnet_count, other_subnet_percent);

    return 0;
}