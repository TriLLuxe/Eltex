#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Вспомогательная функция для копирования строки
static char* copy_string(const char* src) {
    if (!src || strlen(src) == 0) return NULL;
    char* dest = malloc(strlen(src) + 1);
    if (dest) strcpy(dest, src);
    return dest;
}

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

void free_contacts(Contact* contacts, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        free_contact(&contacts[i]);
    }
    free(contacts);
}
// Добавление контакта
int add_contact(Contact** contacts, unsigned int* count, 
    const char* last_name, const char* first_name, const char* middle_name,
    const char* workplace, const char* position,
    const char* phone, const char* email, 
    const char* social_link, const char* messenger_profile) {
Contact* temp = realloc(*contacts, (*count + 1) * sizeof(Contact));
if (!temp) return -1; // Ошибка выделения памяти
*contacts = temp;
Contact* new_contact = &(*contacts)[*count];

new_contact->name.last_name = copy_string(last_name ? last_name : "");
new_contact->name.first_name = copy_string(first_name ? first_name : "");
new_contact->name.middle_name = copy_string(middle_name);
new_contact->workplace = copy_string(workplace);
new_contact->position = copy_string(position);
new_contact->details.phone = copy_string(phone);
new_contact->details.email = copy_string(email);
new_contact->details.social_link = copy_string(social_link);
new_contact->details.messenger_profile = copy_string(messenger_profile);

(*count)++;
return 0; 
}
// Редактирование существующего контакта
int edit_contact(Contact* contacts, unsigned int count, unsigned int id,
    const char* last_name, const char* first_name, const char* middle_name,
    const char* workplace, const char* position,
    const char* phone, const char* email, 
    const char* social_link, const char* messenger_profile) {
if (id >= count) return -1; // Контакт не найден
Contact* contact = &contacts[id];

if (last_name && strlen(last_name) > 0) {
free(contact->name.last_name);
contact->name.last_name = copy_string(last_name);
}
if (first_name && strlen(first_name) > 0) {
free(contact->name.first_name);
contact->name.first_name = copy_string(first_name);
}
if (middle_name) {
free(contact->name.middle_name);
contact->name.middle_name = copy_string(middle_name);
}
if (workplace) {
free(contact->workplace);
contact->workplace = copy_string(workplace);
}
if (position) {
free(contact->position);
contact->position = copy_string(position);
}
if (phone) {
free(contact->details.phone);
contact->details.phone = copy_string(phone);
}
if (email) {
free(contact->details.email);
contact->details.email = copy_string(email);
}
if (social_link) {
free(contact->details.social_link);
contact->details.social_link = copy_string(social_link);
}
if (messenger_profile) {
free(contact->details.messenger_profile);
contact->details.messenger_profile = copy_string(messenger_profile);
}

return 0; 
}
// Удаление контакта
int delete_contact(Contact** contacts, unsigned int* count, unsigned int id) {
    if (id >= *count) return -1; // Контакт не найден
    free_contact(&(*contacts)[id]);
    for (unsigned int i = id; i < *count - 1; i++) {
        (*contacts)[i] = (*contacts)[i + 1];
    }
    (*count)--;
    Contact* temp = realloc(*contacts, *count * sizeof(Contact));
    if (temp || *count == 0) *contacts = temp;
    return 0; 
}
// Сохранение контактов в файл
int save_to_file(const Contact* contacts, unsigned int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return -1; // Ошибка открытия файла
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
    return 0; 
}
// Загрузка контактов из файла
int load_from_file(Contact** contacts, unsigned int* count, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1; // Файл не найден
    free_contacts(*contacts, *count);
    *contacts = NULL;
    *count = 0;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        Contact* temp = realloc(*contacts, (*count + 1) * sizeof(Contact));
        if (!temp) {
            fclose(file);
            return -1; // Ошибка выделения памяти
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
    return 0; 
}
// Вывод контактов в буфер
void print_contacts(const Contact* contacts, unsigned int count, char* output, unsigned int output_size) {
    output[0] = '\0'; 
    if (count == 0) {
        strncat(output, "Список контактов пуст.\n", output_size - strlen(output) - 1);
        return;
    }

    for (unsigned int i = 0; i < count; i++) {
        const Contact* c = &contacts[i];
        char buffer[1024];
        snprintf(buffer, sizeof(buffer),
                 "Контакт ID %u:\n"
                 "  Фамилия: %s\n"
                 "  Имя: %s\n"
                 "  Отчество: %s\n"
                 "  Место работы: %s\n"
                 "  Должность: %s\n"
                 "  Телефон: %s\n"
                 "  Email: %s\n"
                 "  Соцсети: %s\n"
                 "  Мессенджер: %s\n"
                 "--------------------\n",
                 i,
                 c->name.last_name ? c->name.last_name : "(не указано)",
                 c->name.first_name ? c->name.first_name : "(не указано)",
                 c->name.middle_name ? c->name.middle_name : "(не указано)",
                 c->workplace ? c->workplace : "(не указано)",
                 c->position ? c->position : "(не указано)",
                 c->details.phone ? c->details.phone : "(не указано)",
                 c->details.email ? c->details.email : "(не указано)",
                 c->details.social_link ? c->details.social_link : "(не указано)",
                 c->details.messenger_profile ? c->details.messenger_profile : "(не указано)");
        strncat(output, buffer, output_size - strlen(output) - 1);
    }
}