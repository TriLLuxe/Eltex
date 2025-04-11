#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 256

static char* input_string(const char* prompt) {
    printf("%s", prompt);
    char buffer[MAX_INPUT];
    if (fgets(buffer, MAX_INPUT, stdin) == NULL) return NULL;
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) == 0) return NULL;
    char* str = malloc(strlen(buffer) + 1);
    if (str) strcpy(str, buffer);
    return str;
}

static void free_input(char* str) {
    if (str) free(str);
}

int main() {
    AVLNode* root = NULL;
    const char* filename = "contacts.txt";

    int result = load_from_file(&root, filename);
    if (result == 1) {
        printf("Файл %s не найден, начинаем с пустого списка.\n", filename);
    } else if (result != 0) {
        printf("Ошибка загрузки контактов из файла %s.\n", filename);
    }

    while (1) {
        printf("\nМеню:\n");
        printf("1. Добавить контакт\n");
        printf("2. Редактировать контакт\n");
        printf("3. Удалить контакт\n");
        printf("4. Сохранить контакты в файл\n");
        printf("5. Загрузить контакты из файла\n");
        printf("6. Вывести все контакты\n");
        printf("7. Выход\n");
        printf("Выберите действие: ");

        int choice;
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                char* last_name = input_string("Введите фамилию: ");
                char* first_name = input_string("Введите имя: ");
                char* middle_name = input_string("Введите отчество (Enter, если нет): ");
                char* workplace = input_string("Введите место работы (Enter, если нет): ");
                char* position = input_string("Введите должность (Enter, если нет): ");
                char* phone = input_string("Введите номер телефона (Enter, если нет): ");
                char* email = input_string("Введите email (Enter, если нет): ");
                char* social_link = input_string("Введите ссылку на соцсети (Enter, если нет): ");
                char* messenger_profile = input_string("Введите профиль мессенджера (Enter, если нет): ");

                if (add_contact(&root, last_name, first_name, middle_name, workplace, position,
                                phone, email, social_link, messenger_profile) == 0) {
                    printf("Контакт успешно добавлен.\n");
                } else {
                    printf("Ошибка при добавлении контакта!\n");
                }

                free_input(last_name);
                free_input(first_name);
                free_input(middle_name);
                free_input(workplace);
                free_input(position);
                free_input(phone);
                free_input(email);
                free_input(social_link);
                free_input(messenger_profile);
                break;
            }
            case 2: {
                char* id_str = input_string("Введите ID контакта для редактирования: ");
                if (!id_str) {
                    printf("Неверный ввод.\n");
                    break;
                }
                int id = atoi(id_str);
                free_input(id_str);
                AVLNode* node = find_node_by_id(root, id);
                if (!node) {
                    printf("Контакт с ID %d не найден!\n", id);
                    break;
                }
                char* new_last_name = input_string("Новая фамилия (Enter, чтобы не менять): ");
                char* new_first_name = input_string("Новое имя (Enter, чтобы не менять): ");
                char* new_middle_name = input_string("Новое отчество (Enter, чтобы не менять): ");
                char* new_workplace = input_string("Новое место работы (Enter, чтобы не менять): ");
                char* new_position = input_string("Новая должность (Enter, чтобы не менять): ");
                char* new_phone = input_string("Новый номер телефона (Enter, чтобы не менять): ");
                char* new_email = input_string("Новый email (Enter, чтобы не менять): ");
                char* new_social_link = input_string("Новая ссылка на соцсети (Enter, чтобы не менять): ");
                char* new_messenger_profile = input_string("Новый профиль мессенджера (Enter, чтобы не менять): ");

                if (edit_contact(&root, id, new_last_name, new_first_name, new_middle_name,
                                 new_workplace, new_position, new_phone, new_email,
                                 new_social_link, new_messenger_profile) == 0) {
                    printf("Контакт обновлен.\n");
                } else {
                    printf("Ошибка при редактировании контакта с ID %d!\n", id);
                }

                free_input(new_last_name);
                free_input(new_first_name);
                free_input(new_middle_name);
                free_input(new_workplace);
                free_input(new_position);
                free_input(new_phone);
                free_input(new_email);
                free_input(new_social_link);
                free_input(new_messenger_profile);
                break;
            }
            case 3: {
                char* id_str = input_string("Введите ID контакта для удаления: ");
                if (!id_str) {
                    printf("Неверный ввод.\n");
                    break;
                }
                int id = atoi(id_str);
                free_input(id_str);
                if (delete_contact(&root, id) == 0) {
                    printf("Контакт удален.\n");
                } else {
                    printf("Контакт с ID %d не найден!\n", id);
                }
                break;
            }
            case 4:
                if (save_to_file(root, filename) == 0) {
                    printf("Контакты сохранены в файл %s\n", filename);
                } else {
                    printf("Ошибка открытия файла!\n");
                }
                break;
            case 5:
                if (load_from_file(&root, filename) == 0) {
                    printf("Контакты загружены из файла %s\n", filename);
                } else {
                    printf("Файл %s не найден, начинаем с пустого списка.\n", filename);
                }
                break;
            case 6: {
                char output[4096];
                output[0] = '\0';
                print_contacts(root, output, sizeof(output));
                if (strlen(output) == 0) {
                    printf("Список контактов пуст.\n");
                } else {
                    printf("%s", output);
                }
                printf("\n\n");
                print_tree(root,0);
                break;
            }
            case 7:
            if (save_to_file(root, filename) == 0) {
                printf("Контакты сохранены в файл %s\n", filename);
            } else {
                printf("Ошибка сохранения контактов.\n");
            }
            free_tree(root);
            printf("Выход из программы.\n");
            return 0;
            default:
                printf("Неверный выбор, попробуйте снова.\n");
        }
    }
    return 0;
}