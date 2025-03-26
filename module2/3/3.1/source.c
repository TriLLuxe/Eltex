#include "header.h"
#include <string.h>
#include <stdio.h>

// Преобразование буквенного формата в битовый
void letter_to_bits(const char *letters, char *bits) {
    for (int i = 0; i < 9; i++) {
        bits[i] = (letters[i] != '-') ? '1' : '0';
    }
    bits[9] = '\0';
}

// Преобразование цифрового формата в битовый
void number_to_bits(const char *number, char *bits) {
    int num = atoi(number);
    int owner = (num / 100) % 10;
    int group = (num / 10) % 10;
    int others = num % 10;

    bits[0] = (owner & 4) ? '1' : '0'; // r для владельца
    bits[1] = (owner & 2) ? '1' : '0'; // w для владельца
    bits[2] = (owner & 1) ? '1' : '0'; // x для владельца
    bits[3] = (group & 4) ? '1' : '0'; // r для группы
    bits[4] = (group & 2) ? '1' : '0'; // w для группы
    bits[5] = (group & 1) ? '1' : '0'; // x для группы
    bits[6] = (others & 4) ? '1' : '0'; // r для остальных
    bits[7] = (others & 2) ? '1' : '0'; // w для остальных
    bits[8] = (others & 1) ? '1' : '0'; // x для остальных
    bits[9] = '\0';
}

// Преобразование битового формата в буквенный
void bits_to_letters(const char *bits, char *letters) {
    letters[0] = bits[0] == '1' ? 'r' : '-';
    letters[1] = bits[1] == '1' ? 'w' : '-';
    letters[2] = bits[2] == '1' ? 'x' : '-';
    letters[3] = bits[3] == '1' ? 'r' : '-';
    letters[4] = bits[4] == '1' ? 'w' : '-';
    letters[5] = bits[5] == '1' ? 'x' : '-';
    letters[6] = bits[6] == '1' ? 'r' : '-';
    letters[7] = bits[7] == '1' ? 'w' : '-';
    letters[8] = bits[8] == '1' ? 'x' : '-';
    letters[9] = '\0';
}

// Преобразование битового формата в цифровой
void bits_to_number(const char *bits, char *number) {
    int owner = (bits[0] == '1' ? 4 : 0) + (bits[1] == '1' ? 2 : 0) + (bits[2] == '1' ? 1 : 0);
    int group = (bits[3] == '1' ? 4 : 0) + (bits[4] == '1' ? 2 : 0) + (bits[5] == '1' ? 1 : 0);
    int others = (bits[6] == '1' ? 4 : 0) + (bits[7] == '1' ? 2 : 0) + (bits[8] == '1' ? 1 : 0);
    sprintf(number, "%d%d%d", owner, group, others);
}

// Получение прав доступа к файлу
int get_file_permissions(const char *filename, char *bits) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        return -1;
    }
    mode_t mode = st.st_mode & 0777; // Берем только права доступа
    bits[0] = (mode & S_IRUSR) ? '1' : '0';
    bits[1] = (mode & S_IWUSR) ? '1' : '0';
    bits[2] = (mode & S_IXUSR) ? '1' : '0';
    bits[3] = (mode & S_IRGRP) ? '1' : '0';
    bits[4] = (mode & S_IWGRP) ? '1' : '0';
    bits[5] = (mode & S_IXGRP) ? '1' : '0';
    bits[6] = (mode & S_IROTH) ? '1' : '0';
    bits[7] = (mode & S_IWOTH) ? '1' : '0';
    bits[8] = (mode & S_IXOTH) ? '1' : '0';
    bits[9] = '\0';
    return 0;
}

//  изменения прав доступа (в нашем случае симуляция)
void apply_modification(const char *command, char *bits) {
    // Флаги для категорий: u, g, o, a
    int apply_to[4] = {0}; // 0 - не применять, 1 - применять

    // Находим оператор (+, -, =)
    const char *op_ptr = strpbrk(command, "+-=");
    if (op_ptr == NULL) {
        fprintf(stderr, "Ошибка: оператор не найден\n");
        return;
    }

    char op = *op_ptr;           // Оператор
    const char *perms = op_ptr + 1; // Права после оператора

    // Определяем категории
    if (op_ptr == command) {
        // Категории не указаны, применяем ко всем (a)
        apply_to[3] = 1;
    } else {
        // Разбираем категории перед оператором
        for (const char *p = command; p < op_ptr; p++) {
            switch (*p) {
                case 'u': apply_to[0] = 1; break;
                case 'g': apply_to[1] = 1; break;
                case 'o': apply_to[2] = 1; break;
                case 'a': apply_to[3] = 1; break;
                default:
                    fprintf(stderr, "Ошибка: неверная категория '%c'\n", *p);
                    return;
            }
        }
    }

    // Если есть 'a', применяем ко всем категориям
    if (apply_to[3]) {
        apply_to[0] = apply_to[1] = apply_to[2] = 1;
    }

    // Позиции битов для категорий: u (0-2), g (3-5), o (6-8)
    int ranges[3][2] = {{0, 3}, {3, 6}, {6, 9}};

    // Если оператор '=', сбрасываем биты в выбранных категориях
    if (op == '=') {
        for (int r = 0; r < 3; r++) {
            if (apply_to[r]) {
                for (int i = ranges[r][0]; i < ranges[r][1]; i++) {
                    bits[i] = '0';
                }
            }
        }
    }

    // Применяем права
    for (const char *p = perms; *p != '\0'; p++) {
        int bit_offset;
        switch (*p) {
            case 'r': bit_offset = 0; break;
            case 'w': bit_offset = 1; break;
            case 'x': bit_offset = 2; break;
            default:
                fprintf(stderr, "Ошибка: неверное право '%c'\n", *p);
                continue;
        }

        // Применяем к выбранным категориям
        for (int r = 0; r < 3; r++) {
            if (apply_to[r]) {
                int bit_pos = ranges[r][0] + bit_offset;
                if (op == '+') bits[bit_pos] = '1';
                else if (op == '-') bits[bit_pos] = '0';
                else if (op == '=') bits[bit_pos] = '1';
            }
        }
    }
}