#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int next_id = 0;  // Следующий доступный ID для новых контактов

static char* copy_string(const char* src) {
    if (!src || strlen(src) == 0) return NULL;
    char* dest = malloc(strlen(src) + 1);
    if (dest) strcpy(dest, src);
    return dest;
}
void copy_contact(Contact* dest, const Contact* src) {
    dest->id = src->id;
    dest->name.last_name = copy_string(src->name.last_name);
    dest->name.first_name = copy_string(src->name.first_name);
    dest->name.middle_name = copy_string(src->name.middle_name);
    dest->workplace = copy_string(src->workplace);
    dest->position = copy_string(src->position);
    dest->details.phone = copy_string(src->details.phone);
    dest->details.email = copy_string(src->details.email);
    dest->details.social_link = copy_string(src->details.social_link);
    dest->details.messenger_profile = copy_string(src->details.messenger_profile);
}
AVLNode* find_min(AVLNode* node) {
    while (node->left) node = node->left;
    return node;
}
int get_height(AVLNode* node) {
    return node ? node->height : 0;
}

int get_balance(AVLNode* node) {
    return node ? get_height(node->left) - get_height(node->right) : 0;
}

int choose_direction(const char* new_last_name, const char* new_first_name,
                     const char* current_last_name, const char* current_first_name) {
    int last_name_cmp = strcmp(new_last_name, current_last_name);
    if (last_name_cmp != 0) {
        return last_name_cmp;
    }
    return strcmp(new_first_name, current_first_name);
}

AVLNode* rotate_right(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = 1 + (get_height(y->left) > get_height(y->right) ? get_height(y->left) : get_height(y->right));
    x->height = 1 + (get_height(x->left) > get_height(x->right) ? get_height(x->left) : get_height(x->right));
    return x;
}

AVLNode* rotate_left(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = 1 + (get_height(x->left) > get_height(x->right) ? get_height(x->left) : get_height(x->right));
    y->height = 1 + (get_height(y->left) > get_height(y->right) ? get_height(y->left) : get_height(y->right));
    return y;
}

AVLNode* create_node(const Contact* contact) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (node) {
        node->contact.id = contact->id;  // Копируем ID
        node->contact.name.last_name = copy_string(contact->name.last_name);
        node->contact.name.first_name = copy_string(contact->name.first_name);
        node->contact.name.middle_name = copy_string(contact->name.middle_name);
        node->contact.workplace = copy_string(contact->workplace);
        node->contact.position = copy_string(contact->position);
        node->contact.details.phone = copy_string(contact->details.phone);
        node->contact.details.email = copy_string(contact->details.email);
        node->contact.details.social_link = copy_string(contact->details.social_link);
        node->contact.details.messenger_profile = copy_string(contact->details.messenger_profile);
        node->left = NULL;
        node->right = NULL;
        node->height = 1;
    }
    return node;
}
AVLNode* delete_node(AVLNode* root, const char* last_name, const char* first_name) {
    if (!root) return root;

    // Сравнение для определения направления поиска
    int cmp = choose_direction(last_name, first_name,
                               root->contact.name.last_name, root->contact.name.first_name);
    
    if (cmp < 0) {
        root->left = delete_node(root->left, last_name, first_name);
    } else if (cmp > 0) {
        root->right = delete_node(root->right, last_name, first_name);
    } else {
        // Узел для удаления найден
        if (!root->left || !root->right) {
            // Случай: нет дочерних узлов или один дочерний узел
            AVLNode* temp = root->left ? root->left : root->right;
            if (!temp) {
                // Нет дочерних узлов
                free_contact(&root->contact); // Освобождаем данные контакта
                free(root);                   // Освобождаем узел
                root = NULL;
            } else {
                // Один дочерний узел
                AVLNode* child = temp;
                *root = *child;           // Копируем данные из дочернего узла
                free(child);              // Освобождаем память дочернего узла
            }
        } else {
            // Случай: два дочерних узла
            AVLNode* temp = find_min(root->right); // Находим минимальный узел в правом поддереве
            free_contact(&root->contact);          // Освобождаем текущий контакт
            copy_contact(&root->contact, &temp->contact); 
            root->right = delete_node(root->right, temp->contact.name.last_name,
                                      temp->contact.name.first_name); // Удаляем минимальный узел
        }
    }

    if (!root) return root;

    // Обновление высоты и балансировка дерева
    root->height = 1 + (get_height(root->left) > get_height(root->right) ?
                        get_height(root->left) : get_height(root->right));
    int balance = get_balance(root);

    // Выполняем вращения для балансировки
    if (balance > 1 && get_balance(root->left) >= 0) {
        return rotate_right(root);
    }
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    if (balance < -1 && get_balance(root->right) <= 0) {
        return rotate_left(root);
    }
    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    return root;
}

AVLNode* insert(AVLNode* node, const Contact* contact) {
    if (!node) return create_node(contact);

    int direction = choose_direction(contact->name.last_name, contact->name.first_name,
                                     node->contact.name.last_name, node->contact.name.first_name);
    if (direction < 0) {
        node->left = insert(node->left, contact);
    } else if (direction > 0) {
        node->right = insert(node->right, contact);
    } else {
        return node; // Дубликат (одинаковые фамилия и имя)
    }

    node->height = 1 + (get_height(node->left) > get_height(node->right) ?
                        get_height(node->left) : get_height(node->right));
    int balance = get_balance(node);

    if (balance > 1 && choose_direction(contact->name.last_name, contact->name.first_name,
                                        node->left->contact.name.last_name,
                                        node->left->contact.name.first_name) < 0) {
        return rotate_right(node);
    }
    if (balance < -1 && choose_direction(contact->name.last_name, contact->name.first_name,
                                         node->right->contact.name.last_name,
                                         node->right->contact.name.first_name) > 0) {
        return rotate_left(node);
    }
    if (balance > 1 && choose_direction(contact->name.last_name, contact->name.first_name,
                                        node->left->contact.name.last_name,
                                        node->left->contact.name.first_name) > 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (balance < -1 && choose_direction(contact->name.last_name, contact->name.first_name,
                                         node->right->contact.name.last_name,
                                         node->right->contact.name.first_name) < 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;
}





AVLNode* find_node(AVLNode* node, const char* last_name, const char* first_name) {
    if (!node) return NULL;

    int cmp = choose_direction(last_name, first_name,
                               node->contact.name.last_name, node->contact.name.first_name);
    if (cmp == 0) return node;
    if (cmp < 0) return find_node(node->left, last_name, first_name);
    return find_node(node->right, last_name, first_name);
}

AVLNode* find_node_by_id(AVLNode* node, int id) {
    if (!node) return NULL;
    if (node->contact.id == id) return node;
    AVLNode* left_result = find_node_by_id(node->left, id);
    if (left_result) return left_result;
    return find_node_by_id(node->right, id);
}

void free_tree(AVLNode* node) {
    if (node) {
        free_tree(node->left);
        free_tree(node->right);
        free_contact(&node->contact);
        free(node);
    }
}

int add_contact(AVLNode** root, const char* last_name, const char* first_name, const char* middle_name,
                const char* workplace, const char* position, const char* phone, const char* email,
                const char* social_link, const char* messenger_profile) {
    Contact new_contact = {
        .id = next_id++,  // Присваиваем уникальный ID и увеличиваем счетчик
        .name = {
            .last_name = copy_string(last_name ? last_name : ""),
            .first_name = copy_string(first_name ? first_name : ""),
            .middle_name = copy_string(middle_name)
        },
        .workplace = copy_string(workplace),
        .position = copy_string(position),
        .details = {
            .phone = copy_string(phone),
            .email = copy_string(email),
            .social_link = copy_string(social_link),
            .messenger_profile = copy_string(messenger_profile)
        }
    };
    *root = insert(*root, &new_contact);
    free_contact(&new_contact);
    return 0;
}

int edit_contact(AVLNode** root, int id, const char* new_last_name,
                 const char* new_first_name, const char* new_middle_name, const char* new_workplace,
                 const char* new_position, const char* new_phone, const char* new_email,
                 const char* new_social_link, const char* new_messenger_profile) {
    AVLNode* node = find_node_by_id(*root, id);
    if (!node) return -1;

    Contact* contact = &node->contact;
    int key_changed = (new_last_name && strcmp(new_last_name, contact->name.last_name) != 0) ||
                      (new_first_name && strcmp(new_first_name, contact->name.first_name) != 0);

    if (key_changed) {
        Contact new_contact = {
            .id = contact->id,
            .name = {
                .last_name = copy_string(new_last_name ? new_last_name : contact->name.last_name),
                .first_name = copy_string(new_first_name ? new_first_name : contact->name.first_name),
                .middle_name = copy_string(new_middle_name ? new_middle_name : contact->name.middle_name)
            },
            .workplace = copy_string(new_workplace ? new_workplace : contact->workplace),
            .position = copy_string(new_position ? new_position : contact->position),
            .details = {
                .phone = copy_string(new_phone ? new_phone : contact->details.phone),
                .email = copy_string(new_email ? new_email : contact->details.email),
                .social_link = copy_string(new_social_link ? new_social_link : contact->details.social_link),
                .messenger_profile = copy_string(new_messenger_profile ? new_messenger_profile : contact->details.messenger_profile)
            }
        };
        *root = delete_node(*root, contact->name.last_name, contact->name.first_name);
        *root = insert(*root, &new_contact);
        free_contact(&new_contact);
    } else {
        if (new_middle_name) {
            free(contact->name.middle_name);
            contact->name.middle_name = copy_string(new_middle_name);
        }
        if (new_workplace) {
            free(contact->workplace);
            contact->workplace = copy_string(new_workplace);
        }
        if (new_position) {
            free(contact->position);
            contact->position = copy_string(new_position);
        }
        if (new_phone) {
            free(contact->details.phone);
            contact->details.phone = copy_string(new_phone);
        }
        if (new_email) {
            free(contact->details.email);
            contact->details.email = copy_string(new_email);
        }
        if (new_social_link) {
            free(contact->details.social_link);
            contact->details.social_link = copy_string(new_social_link);
        }
        if (new_messenger_profile) {
            free(contact->details.messenger_profile);
            contact->details.messenger_profile = copy_string(new_messenger_profile);
        }
    }
    return 0;
}

int delete_contact(AVLNode** root, int id) {
    AVLNode* node = find_node_by_id(*root, id);
    if (!node) return -1;
    *root = delete_node(*root, node->contact.name.last_name, node->contact.name.first_name);
    return 0;
}


static void save_to_file_helper(const AVLNode* node, FILE* file) {
    if (!node) return;
    save_to_file_helper(node->left, file);
    const Contact* c = &node->contact;
    fprintf(file, "%d,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
            c->id,
            c->name.last_name ? c->name.last_name : "",
            c->name.first_name ? c->name.first_name : "",
            c->name.middle_name ? c->name.middle_name : "",
            c->workplace ? c->workplace : "",
            c->position ? c->position : "",
            c->details.phone ? c->details.phone : "",
            c->details.email ? c->details.email : "",
            c->details.social_link ? c->details.social_link : "",
            c->details.messenger_profile ? c->details.messenger_profile : "");
    save_to_file_helper(node->right, file);
}

int save_to_file(const AVLNode* root, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return -1;
    save_to_file_helper(root, file);
    fclose(file);
    return 0;
}

int load_from_file(AVLNode** root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;
    free_tree(*root);
    *root = NULL;

    int max_id = 0;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char* token = strtok(line, ",");
        int id = token ? atoi(token) : 0;
        if (id > max_id) max_id = id;
        token = strtok(NULL, ",");
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

        Contact new_contact = {
            .id = id,
            .name = {
                .last_name = last_name,
                .first_name = first_name,
                .middle_name = middle_name
            },
            .workplace = workplace,
            .position = position,
            .details = {
                .phone = phone,
                .email = email,
                .social_link = social_link,
                .messenger_profile = messenger_profile
            }
        };
        *root = insert(*root, &new_contact);
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
    next_id = max_id + 1;  // Обновляем next_id до следующего доступного значения
    return 0;
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

void print_contacts(const AVLNode* root, char* output, unsigned int output_size) {
    if (!root) return;
    print_contacts(root->left, output, output_size);
    const Contact* c = &root->contact;
    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
             "Контакт ID: %d\n"
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
             c->id,
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
    print_contacts(root->right, output, output_size);
}
void print_tree(AVLNode* node, int level) {
    if (!node) return;
    print_tree(node->left, level + 1);
    for (int i = 0; i < level; i++) printf("    ");
    printf("%s %s (ID: %d, высота: %d, баланс: %d)\n",
           node->contact.name.last_name, node->contact.name.first_name,
           node->contact.id, node->height, get_balance(node));
    print_tree(node->right, level + 1);
}