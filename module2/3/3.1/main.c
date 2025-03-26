#include "header.h"
#include <stdio.h>
#include <string.h>

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
        printf("Вот примеры вызовов:\n"
            "./main rwxr-xr-x\n"
            "./main 755\n"
            "./main rw-r--r-- test.txt\n"
            "./main 644 script.sh\n"
            "./main chmod u+x test.txt\n"
            "./main chmod ug=rwx test.txt\n"
            "./main chmod a-w test.txt\n"
            "./main chmod +x test.txt\n");
        return 1;
    }
    

    char bits[10];

    // 1. Пользователь ввел требуемые права доступа
    if (strlen(argv[1]) == 3 ) {
        number_to_bits(argv[1], bits);
        print_permissions(bits);
    } else if (strlen(argv[1]) == 9) {
        letter_to_bits(argv[1], bits);
        print_permissions(bits);
    }

    // 2. Права доступа к файлу
    if (argc >= 3 ) {
        if (get_file_permissions(argv[2], bits) == -1 && get_file_permissions(argv[3], bits)) {
            printf("Ошибка: файл '%s' не найден\n", argv[2]);
            return 1;
        }
        printf("\nПрава доступа к файлу %s:\n", argv[2]);
        print_permissions(bits);
    }

    // 3. Симуляция изменения прав
    if (argc >= 4 && strcmp(argv[1], "chmod") == 0) {
        apply_modification(argv[2], bits);
        printf("\nОбновленные права после '%s':\n", argv[2]);
        print_permissions(bits);
    }

    return 0;
}