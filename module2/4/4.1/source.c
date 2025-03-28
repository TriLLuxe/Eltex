#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* copy_string(const char* src) {
    if (!src || strlen(src) == 0) return NULL;
    char* dest = malloc(strlen(src) + 1);
    if (dest) strcpy(dest, src);
    return dest;
}

static int compare_contacts(const Contact* a, const Contact* b) {
    if (a->name.last_name && b->name.last_name) {
        return strcmp(a->name.last_name, b->name.last_name);
    } else if (a->name.last_name) {
        return 1;
    } else if (b->name.last_name) {
        return -1;
    } else {
        return 0;
    }
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

void free_list(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        free_contact(&temp->contact);
        free(temp);
    }
}

int add_contact(Node** head,
                const char* last_name, const char* first_name, const char* middle_name,
                const char* workplace, const char* position,
                const char* phone, const char* email,
                const char* social_link, const char* messenger_profile) {
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) return -1;

    new_node->contact.name.last_name = copy_string(last_name ? last_name : "");
    new_node->contact.name.first_name = copy_string(first_name ? first_name : "");
    new_node->contact.name.middle_name = copy_string(middle_name);
    new_node->contact.workplace = copy_string(workplace);
    new_node->contact.position = copy_string(position);
    new_node->contact.details.phone = copy_string(phone);
    new_node->contact.details.email = copy_string(email);
    new_node->contact.details.social_link = copy_string(social_link);
    new_node->contact.details.messenger_profile = copy_string(messenger_profile);
    new_node->next = NULL;
    new_node->prev = NULL;

    if (!*head) {
        *head = new_node;
        return 0;
    }

    Node* current = *head;
    while (current && compare_contacts(&current->contact, &new_node->contact) < 0) {
        current = current->next;
    }

    if (current) {
        new_node->next = current;
        new_node->prev = current->prev;
        if (current->prev) {
            current->prev->next = new_node;
        } else {
            *head = new_node;
        }
        current->prev = new_node;
    } else {
        Node* last = *head;
        while (last->next) last = last->next;
        last->next = new_node;
        new_node->prev = last;
    }
    return 0;
}

int edit_contact(Node** head, unsigned int id,
                 const char* last_name, const char* first_name, const char* middle_name,
                 const char* workplace, const char* position,
                 const char* phone, const char* email,
                 const char* social_link, const char* messenger_profile) {
    Node* current = *head;
    unsigned int index = 0;

    while (current) {
        if (index == id) {
            // Проверка, изменилась ли фамилия
            int last_name_changed = (last_name && strlen(last_name) > 0 &&
                                     strcmp(last_name, current->contact.name.last_name) != 0);
            if (last_name && strlen(last_name) > 0) {
                free(current->contact.name.last_name);
                current->contact.name.last_name = copy_string(last_name);
            }
            if (first_name && strlen(first_name) > 0) {
                free(current->contact.name.first_name);
                current->contact.name.first_name = copy_string(first_name);
            }
            if (middle_name) {
                free(current->contact.name.middle_name);
                current->contact.name.middle_name = copy_string(middle_name);
            }
            if (workplace) {
                free(current->contact.workplace);
                current->contact.workplace = copy_string(workplace);
            }
            if (position) {
                free(current->contact.position);
                current->contact.position = copy_string(position);
            }
            if (phone) {
                free(current->contact.details.phone);
                current->contact.details.phone = copy_string(phone);
            }
            if (email) {
                free(current->contact.details.email);
                current->contact.details.email = copy_string(email);
            }
            if (social_link) {
                free(current->contact.details.social_link);
                current->contact.details.social_link = copy_string(social_link);
            }
            if (messenger_profile) {
                free(current->contact.details.messenger_profile);
                current->contact.details.messenger_profile = copy_string(messenger_profile);
            }

            // Если фамилия изменилась, удаляем узел и добавляем заново
            if (last_name_changed) {
                Node* temp = current;
                if (current->prev) {
                    current->prev->next = current->next;
                } else {
                    *head = current->next;
                }
                if (current->next) {
                    current->next->prev = current->prev;
                }

                // Добавляем контакт в правильное место
                add_contact(head, temp->contact.name.last_name, temp->contact.name.first_name,
                            temp->contact.name.middle_name, temp->contact.workplace, temp->contact.position,
                            temp->contact.details.phone, temp->contact.details.email,
                            temp->contact.details.social_link, temp->contact.details.messenger_profile);

                free_contact(&temp->contact);
                free(temp);
            }
            return 0;
        }
        current = current->next;
        index++;
    }
    return -1;
}

int delete_contact(Node** head, unsigned int id) {
    Node* current = *head;
    unsigned int index = 0;
    while (current) {
        if (index == id) {
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                *head = current->next;
            }
            if (current->next) {
                current->next->prev = current->prev;
            }
            free_contact(&current->contact);
            free(current);
            return 0;
        }
        current = current->next;
        index++;
    }
    return -1;
}

int save_to_file(const Node* head, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return -1;
    const Node* current = head;
    while (current) {
        const Contact* c = &current->contact;
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
        current = current->next;
    }
    fclose(file);
    return 0;
}

int load_from_file(Node** head, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;
    free_list(*head);
    *head = NULL;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char* token = strtok(line, ",");
        char* last_name = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        char* first_name = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        char* middle_name = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        char* workplace = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        char* position = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        char* phone = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        char* email = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        char* social_link = token ? strdup(token) : NULL;
        token = strtok(NULL, ",");
        char* messenger_profile = token ? strdup(token) : NULL;

        add_contact(head, last_name, first_name, middle_name, workplace, position,
                    phone, email, social_link, messenger_profile);

        free(last_name);
        free(first_name);
        free(middle_name);
        free(workplace);
        free(position);
        free(phone);
        free(email);
        free(social_link);
        free(messenger_profile);
    }
    fclose(file);
    return 0;
}

void print_contacts(const Node* head, char* output, unsigned int output_size) {
    output[0] = '\0';
    if (!head) {
        strncat(output, "Список контактов пуст.\n", output_size - strlen(output) - 1);
        return;
    }
    const Node* current = head;
    unsigned int id = 0;
    while (current) {
        const Contact* c = &current->contact;
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
                 id,
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
        current = current->next;
        id++;
    }
}