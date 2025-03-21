#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INPUT 256
// Вспомогательная функция
static char* input_string(const char* prompt) {
    printf("%s", prompt);
    char buffer[MAX_INPUT];
    if (fgets(buffer, MAX_INPUT, stdin) == NULL) return NULL;
    buffer[strcspn(buffer, "\n")] = 0; // Удаляем символ переноса строки
    if (strlen(buffer) == 0) return NULL;
    char* str = malloc(strlen(buffer) + 1);
    strcpy(str, buffer);
    return str;
}
// Освобождение памяти для одного контакта
void free_contact(Contact* contact) {
    free(contact->name.last_name);
    free(contact->name.first_name);
    free(contact->name.middle_name);
    free(contact->workplace);
    free(contact->position);
    free(contact->details.phone);
    free(contact->details.email);
    free(contact->details.social_link);
    free(contact->details.messenger_profile);
}
// Освобождение памяти для всех контактов
void free_contacts(Contact* contacts, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        free_contact(&contacts[i]);
    }
    free(contacts);
}
// Добавление контакта
void add_contact(Contact** contacts, unsigned int* count) {
    Contact* temp = realloc(*contacts, (*count + 1) * sizeof(Contact));
    if (!temp) {
        printf("Ошибка выделения памяти!\n");
        return;
    }
    *contacts = temp;
    Contact* new_contact = &(*contacts)[*count];

    new_contact->name.last_name = input_string("Введите фамилию: ");
    if (!new_contact->name.last_name) new_contact->name.last_name = strdup("");
    new_contact->name.first_name = input_string("Введите имя: ");
    if (!new_contact->name.first_name) new_contact->name.first_name = strdup("");
    new_contact->name.middle_name = input_string("Введите отчество (Enter, если нет): ");
    new_contact->workplace = input_string("Введите место работы (Enter, если нет): ");
    new_contact->position = input_string("Введите должность (Enter, если нет): ");
    new_contact->details.phone = input_string("Введите номер телефона (Enter, если нет): ");
    new_contact->details.email = input_string("Введите email (Enter, если нет): ");
    new_contact->details.social_link = input_string("Введите ссылку на соцсети (Enter, если нет): ");
    new_contact->details.messenger_profile = input_string("Введите профиль мессенджера (Enter, если нет): ");

    (*count)++;
    printf("Контакт добавлен с ID %u\n", *count - 1);
}
// Редактирование существующего контакта
void edit_contact(Contact* contacts, unsigned int count, unsigned int id) {
    if (id >= count) {
        printf("Контакт с ID %u не найден!\n", id);
        return;
    }
    Contact* contact = &contacts[id];
    printf("Редактирование контакта ID %u. Оставьте поле пустым, чтобы не менять.\n", id);

    char* temp = input_string("Новая фамилия: ");
    if (temp) { free(contact->name.last_name); contact->name.last_name = temp; }
    temp = input_string("Новое имя: ");
    if (temp) { free(contact->name.first_name); contact->name.first_name = temp; }
    temp = input_string("Новое отчество: ");
    if (temp) { free(contact->name.middle_name); contact->name.middle_name = temp; }
    temp = input_string("Новое место работы: ");
    if (temp) { free(contact->workplace); contact->workplace = temp; }
    temp = input_string("Новая должность: ");
    if (temp) { free(contact->position); contact->position = temp; }
    temp = input_string("Новый номер телефона: ");
    if (temp) { free(contact->details.phone); contact->details.phone = temp; }
    temp = input_string("Новый email: ");
    if (temp) { free(contact->details.email); contact->details.email = temp; }
    temp = input_string("Новая ссылка на соцсети: ");
    if (temp) { free(contact->details.social_link); contact->details.social_link = temp; }
    temp = input_string("Новый профиль мессенджера: ");
    if (temp) { free(contact->details.messenger_profile); contact->details.messenger_profile = temp; }

    printf("Контакт обновлен.\n");
}
// Удаление контакта
void delete_contact(Contact** contacts, unsigned int* count, unsigned int id) {
    if (id >= *count) {
        printf("Контакт с ID %u не найден!\n", id);
        return;
    }
    free_contact(&(*contacts)[id]);
    for (unsigned int i = id; i < *count - 1; i++) {
        (*contacts)[i] = (*contacts)[i + 1];
    }
    (*count)--;
    Contact* temp = realloc(*contacts, *count * sizeof(Contact));
    if (temp || *count == 0) *contacts = temp;
    printf("Контакт удален.\n");
}
// Сохранение контактов в файл
void save_to_file(const Contact* contacts, unsigned int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Ошибка открытия файла!\n");
        return;
    }
    for (unsigned int i = 0; i < count; i++) {
        const Contact* c = &contacts[i];
        fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                c->name.last_name ? c->name.last_name : "",
                c->name.first_name ? c->name.first_name : "",
                c->name.middle_name ? c->name.middle_name : "",
                c->workplace ? c->workplace : "",
                c->position ? c->position : "",
                c->details.phone ? c->details.phone : "",
                c->details.email ? c->details.email : "",
                c->details.social_link ? c->details.social_link : "",
                c->details.messenger_profile ? c->details.messenger_profile : "");
    }
    fclose(file);
    printf("Контакты сохранены в файл %s\n", filename);
}
// Загрузка контактов из файла
void load_from_file(Contact** contacts, unsigned int* count, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Файл %s не найден, начинаем с пустого списка.\n", filename);
        return;
    }
    free_contacts(*contacts, *count);
    *contacts = NULL;
    *count = 0;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        Contact* temp = realloc(*contacts, (*count + 1) * sizeof(Contact));
        if (!temp) {
            printf("Ошибка выделения памяти!\n");
            break;
        }
        *contacts = temp;
        Contact* c = &(*contacts)[*count];

        char* token = strtok(line, ",");
        c->name.last_name = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        c->name.first_name = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        c->name.middle_name = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        c->workplace = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        c->position = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        c->details.phone = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        c->details.email = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        c->details.social_link = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        c->details.messenger_profile = token ? strdup(token) : NULL;

        (*count)++;
    }
    fclose(file);
    printf("Контакты загружены из файла %s\n", filename);
}
void print_contacts(const Contact* contacts, unsigned int count) {
    if (count == 0) {
        printf("Список контактов пуст.\n");
        return;
    }

    for (unsigned int i = 0; i < count; i++) {
        const Contact* c = &contacts[i];
        printf("Контакт ID %u:\n", i);
        printf("  Фамилия: %s\n", c->name.last_name ? c->name.last_name : "(не указано)");
        printf("  Имя: %s\n", c->name.first_name ? c->name.first_name : "(не указано)");
        printf("  Отчество: %s\n", c->name.middle_name ? c->name.middle_name : "(не указано)");
        printf("  Место работы: %s\n", c->workplace ? c->workplace : "(не указано)");
        printf("  Должность: %s\n", c->position ? c->position : "(не указано)");
        printf("  Телефон: %s\n", c->details.phone ? c->details.phone : "(не указано)");
        printf("  Email: %s\n", c->details.email ? c->details.email : "(не указано)");
        printf("  Соцсети: %s\n", c->details.social_link ? c->details.social_link : "(не указано)");
        printf("  Мессенджер: %s\n", c->details.messenger_profile ? c->details.messenger_profile : "(не указано)");
        printf("--------------------\n");
    }
}