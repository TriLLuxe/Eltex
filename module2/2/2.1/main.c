#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(1251); // Windows-1251 для вывода
    SetConsoleCP(1251);       // Windows-1251 для ввода
#endif

    Contact* contacts = NULL;
    unsigned int contact_count = 0;
    const char* filename = "contacts.txt";

    load_from_file(&contacts, &contact_count, filename);

    while (1) {
        printf("\nМеню:\n");
        printf("1. Добавить контакт\n");
        printf("2. Редактировать контакт\n");
        printf("3. Удалить контакт\n");
        printf("4. Сохранить контакты в файл\n");
        printf("5. Загрузить контакты из файла\n");
        printf("6. Вывести все контакты\n"); // Новая опция
        printf("7. Выход\n");               // Сдвинули выход на 7
        printf("Выберите действие: ");

        int choice;
        scanf("%d", &choice);
        getchar(); // Удаляем символ переноса строки

        switch (choice) {
            case 1:
                add_contact(&contacts, &contact_count);
                break;
            case 2: {
                unsigned int id;
                printf("Введите ID контакта для редактирования: ");
                scanf("%u", &id);
                getchar();
                edit_contact(contacts, contact_count, id);
                break;
            }
            case 3: {
                unsigned int id;
                printf("Введите ID контакта для удаления: ");
                scanf("%u", &id);
                getchar();
                delete_contact(&contacts, &contact_count, id);
                break;
            }
            case 4:
                save_to_file(contacts, contact_count, filename);
                break;
            case 5:
                load_from_file(&contacts, &contact_count, filename);
                break;
            case 6: // Новая опция
                print_contacts(contacts, contact_count);
                break;
            case 7: // Выход теперь 7
                free_contacts(contacts, contact_count);
                printf("Выход из программы.\n");
                return 0;
            default:
                printf("Неверный выбор, попробуйте снова.\n");
        }
    }
    return 0;
}