#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INPUT 256
// ��������������� �������
static char* input_string(const char* prompt) {
    printf("%s", prompt);
    char buffer[MAX_INPUT];
    if (fgets(buffer, MAX_INPUT, stdin) == NULL) return NULL;
    buffer[strcspn(buffer, "\n")] = 0; // ������� ������ �������� ������
    if (strlen(buffer) == 0) return NULL;
    char* str = malloc(strlen(buffer) + 1);
    strcpy(str, buffer);
    return str;
}
// ������������ ������ ��� ������ ��������
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
// ������������ ������ ��� ���� ���������
void free_contacts(Contact* contacts, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        free_contact(&contacts[i]);
    }
    free(contacts);
}
// ���������� ��������
void add_contact(Contact** contacts, unsigned int* count) {
    Contact* temp = realloc(*contacts, (*count + 1) * sizeof(Contact));
    if (!temp) {
        printf("������ ��������� ������!\n");
        return;
    }
    *contacts = temp;
    Contact* new_contact = &(*contacts)[*count];

    new_contact->name.last_name = input_string("������� �������: ");
    if (!new_contact->name.last_name) new_contact->name.last_name = strdup("");
    new_contact->name.first_name = input_string("������� ���: ");
    if (!new_contact->name.first_name) new_contact->name.first_name = strdup("");
    new_contact->name.middle_name = input_string("������� �������� (Enter, ���� ���): ");
    new_contact->workplace = input_string("������� ����� ������ (Enter, ���� ���): ");
    new_contact->position = input_string("������� ��������� (Enter, ���� ���): ");
    new_contact->details.phone = input_string("������� ����� �������� (Enter, ���� ���): ");
    new_contact->details.email = input_string("������� email (Enter, ���� ���): ");
    new_contact->details.social_link = input_string("������� ������ �� ������� (Enter, ���� ���): ");
    new_contact->details.messenger_profile = input_string("������� ������� ����������� (Enter, ���� ���): ");

    (*count)++;
    printf("������� �������� � ID %u\n", *count - 1);
}
// �������������� ������������� ��������
void edit_contact(Contact* contacts, unsigned int count, unsigned int id) {
    if (id >= count) {
        printf("������� � ID %u �� ������!\n", id);
        return;
    }
    Contact* contact = &contacts[id];
    printf("�������������� �������� ID %u. �������� ���� ������, ����� �� ������.\n", id);

    char* temp = input_string("����� �������: ");
    if (temp) { free(contact->name.last_name); contact->name.last_name = temp; }
    temp = input_string("����� ���: ");
    if (temp) { free(contact->name.first_name); contact->name.first_name = temp; }
    temp = input_string("����� ��������: ");
    if (temp) { free(contact->name.middle_name); contact->name.middle_name = temp; }
    temp = input_string("����� ����� ������: ");
    if (temp) { free(contact->workplace); contact->workplace = temp; }
    temp = input_string("����� ���������: ");
    if (temp) { free(contact->position); contact->position = temp; }
    temp = input_string("����� ����� ��������: ");
    if (temp) { free(contact->details.phone); contact->details.phone = temp; }
    temp = input_string("����� email: ");
    if (temp) { free(contact->details.email); contact->details.email = temp; }
    temp = input_string("����� ������ �� �������: ");
    if (temp) { free(contact->details.social_link); contact->details.social_link = temp; }
    temp = input_string("����� ������� �����������: ");
    if (temp) { free(contact->details.messenger_profile); contact->details.messenger_profile = temp; }

    printf("������� ��������.\n");
}
// �������� ��������
void delete_contact(Contact** contacts, unsigned int* count, unsigned int id) {
    if (id >= *count) {
        printf("������� � ID %u �� ������!\n", id);
        return;
    }
    free_contact(&(*contacts)[id]);
    for (unsigned int i = id; i < *count - 1; i++) {
        (*contacts)[i] = (*contacts)[i + 1];
    }
    (*count)--;
    Contact* temp = realloc(*contacts, *count * sizeof(Contact));
    if (temp || *count == 0) *contacts = temp;
    printf("������� ������.\n");
}
// ���������� ��������� � ����
void save_to_file(const Contact* contacts, unsigned int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("������ �������� �����!\n");
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
    printf("�������� ��������� � ���� %s\n", filename);
}
// �������� ��������� �� �����
void load_from_file(Contact** contacts, unsigned int* count, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("���� %s �� ������, �������� � ������� ������.\n", filename);
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
            printf("������ ��������� ������!\n");
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
    printf("�������� ��������� �� ����� %s\n", filename);
}
void print_contacts(const Contact* contacts, unsigned int count) {
    if (count == 0) {
        printf("������ ��������� ����.\n");
        return;
    }

    for (unsigned int i = 0; i < count; i++) {
        const Contact* c = &contacts[i];
        printf("������� ID %u:\n", i);
        printf("  �������: %s\n", c->name.last_name ? c->name.last_name : "(�� �������)");
        printf("  ���: %s\n", c->name.first_name ? c->name.first_name : "(�� �������)");
        printf("  ��������: %s\n", c->name.middle_name ? c->name.middle_name : "(�� �������)");
        printf("  ����� ������: %s\n", c->workplace ? c->workplace : "(�� �������)");
        printf("  ���������: %s\n", c->position ? c->position : "(�� �������)");
        printf("  �������: %s\n", c->details.phone ? c->details.phone : "(�� �������)");
        printf("  Email: %s\n", c->details.email ? c->details.email : "(�� �������)");
        printf("  �������: %s\n", c->details.social_link ? c->details.social_link : "(�� �������)");
        printf("  ����������: %s\n", c->details.messenger_profile ? c->details.messenger_profile : "(�� �������)");
        printf("--------------------\n");
    }
}