
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
    Node* head = NULL;
    const char* filename = "contacts.txt";

    if (load_from_file(&head, filename) != 0) {
        printf("Файл %s не найден, начинаем с пустого списка.\n", filename);
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

                if (add_contact(&head, last_name, first_name, middle_name, workplace, position,
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
                unsigned int id;
                printf("Введите ID контакта для редактирования: ");
                scanf("%u", &id);
                getchar();

                char* last_name = input_string("Новая фамилия (Enter, чтобы не менять): ");
                char* first_name = input_string("Новое имя (Enter, чтобы не менять): ");
                char* middle_name = input_string("Новое отчество (Enter, чтобы не менять): ");
                char* workplace = input_string("Новое место работы (Enter, чтобы не менять): ");
                char* position = input_string("Новая должность (Enter, чтобы не менять): ");
                char* phone = input_string("Новый номер телефона (Enter, чтобы не менять): ");
                char* email = input_string("Новый email (Enter, чтобы не менять): ");
                char* social_link = input_string("Новая ссылка на соцсети (Enter, чтобы не менять): ");
                char* messenger_profile = input_string("Новый профиль мессенджера (Enter, чтобы не менять): ");

                if (edit_contact(&head, id, last_name, first_name, middle_name, workplace, position,
                                 phone, email, social_link, messenger_profile) == 0) {
                    printf("Контакт обновлен.\n");
                } else {
                    printf("Контакт с ID %u не найден!\n", id);
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
            case 3: {
                unsigned int id;
                printf("Введите ID контакта для удаления: ");
                scanf("%u", &id);
                getchar();

                if (delete_contact(&head, id) == 0) {
                    printf("Контакт удален.\n");
                } else {
                    printf("Контакт с ID %u не найден!\n", id);
                }
                break;
            }
            case 4:
                if (save_to_file(head, filename) == 0) {
                    printf("Контакты сохранены в файл %s\n", filename);
                } else {
                    printf("Ошибка открытия файла!\n");
                }
                break;
            case 5:
                if (load_from_file(&head, filename) == 0) {
                    printf("Контакты загружены из файла %s\n", filename);
                } else {
                    printf("Файл %s не найден, начинаем с пустого списка.\n", filename);
                }
                break;
            case 6: {
                char output[4096] = {0};
                print_contacts(head, output, sizeof(output));
                printf("%s", output);
                break;
            }
            case 7:
                free_list(head);
                printf("Выход из программы.\n");
                return 0;
            default:
                printf("Неверный выбор, попробуйте снова.\n");
        }
    }
    return 0;
}