#include "header.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
// Проверка, является ли строка трёхзначным числом
bool is_three_digit_number(const char *str) {
    if (strlen(str) != 3) return false;
    for (int i = 0; i < 3; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

// Вывод прав доступа 
void print_permissions(const char *bits) {
    char letters[10], number[4];
    bits_to_letters(bits, letters);
    bits_to_number(bits, number);
    printf("Буквенный формат: %s\n", letters);
    printf("Цифровой формат: %s\n", number);
    printf("Битовый формат: %s\n", bits);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s <права или команда> [имя_файла]\n", argv[0]);
        printf("Примеры:\n"
               "./main rwxr-xr-x\n"
               "./main 755\n"
               "./main test.txt\n"
               "./main chmod u+x test.txt\n"
               "./main chmod 657 test.txt\n");
        return 1;
    }

    char bits[10];

    // Обработка случая с одним аргументом
    if (argc == 2) {
        if (strlen(argv[1]) == 3 && is_three_digit_number(argv[1])) {
            number_to_bits(argv[1], bits);
            print_permissions(bits);
        } else if (strlen(argv[1]) == 9) {
            letter_to_bits(argv[1], bits);
            print_permissions(bits);
        } else {
            
            if (get_file_permissions(argv[1], bits) == -1) {
                printf("Ошибка: файл '%s' не найден\n", argv[1]);
                return 1;
            }
            printf("Права доступа к файлу %s:\n", argv[1]);
            print_permissions(bits);
        }
    } 
    // Обработка команды  c chmod
    else if (argc == 4 && strcmp(argv[1], "chmod") == 0) {
        
        if (get_file_permissions(argv[3], bits) == -1) {
            printf("Ошибка: файл '%s' не найден\n", argv[3]);
            return 1;
        }
        printf("Текущие права доступа к файлу %s:\n", argv[3]);
        print_permissions(bits);

        // Применяем изменение прав
        if (is_three_digit_number(argv[2])) {
            number_to_bits(argv[2], bits);
            printf("\nСимуляция установки прав доступа к %s на %s:\n", argv[3], argv[2]);
            print_permissions(bits);
        } else {
            apply_modification(argv[2], bits);
            printf("\nСимуляция применения '%s' к правам файла %s:\n", argv[2], argv[3]);
            print_permissions(bits);
        }
    } 
    // Ошибка при неверных аргументах
    else {
        printf("Неверное количество аргументов или неверная команда.\n");
        printf("Использование: %s <права или команда> [имя_файла]\n", argv[0]);
        printf("Примеры:\n"
               "./main rwxr-xr-x\n"
               "./main 755\n"
               "./main test.txt\n"
               "./main chmod u+x test.txt\n"
               "./main chmod 657 test.txt\n");
        return 1;
    }

    return 0;
}