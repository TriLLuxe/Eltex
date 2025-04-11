#ifndef HEADER_H
#define HEADER_H
#include <stddef.h>

typedef struct {
    char* phone;
    char* email;
    char* social_link;
    char* messenger_profile;
} ContactDetails;

typedef struct {
    char* last_name;
    char* first_name;
    char* middle_name;
} FullName;

typedef struct {
    int id;  
    FullName name;
    char* workplace;
    char* position;
    ContactDetails details;
} Contact;

// Структура узла AVL-дерева
typedef struct AVLNode {
    Contact contact;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

// Функции для работы с AVL-деревом
int get_height(AVLNode* node);
int get_balance(AVLNode* node);
int choose_direction(const char* new_last_name, const char* new_first_name, 
                     const char* current_last_name, const char* current_first_name);
AVLNode* rotate_right(AVLNode* y);
AVLNode* rotate_left(AVLNode* x);
AVLNode* create_node(const Contact* contact);
AVLNode* insert(AVLNode* node, const Contact* contact);
AVLNode* delete_node(AVLNode* root, const char* last_name, const char* first_name);
AVLNode* find_node(AVLNode* node, const char* last_name, const char* first_name);
AVLNode* find_node_by_id(AVLNode* node, int id);
void free_tree(AVLNode* node);

// Функции для работы с контактами
int add_contact(AVLNode** root, const char* last_name, const char* first_name, const char* middle_name,
                const char* workplace, const char* position, const char* phone, const char* email,
                const char* social_link, const char* messenger_profile);
int edit_contact(AVLNode** root, int id, const char* new_last_name,
                 const char* new_first_name, const char* new_middle_name, const char* new_workplace,
                 const char* new_position, const char* new_phone, const char* new_email,
                 const char* new_social_link, const char* new_messenger_profile);
int delete_contact(AVLNode** root, int id);
int save_to_file(const AVLNode* root, const char* filename);
int load_from_file(AVLNode** root, const char* filename);
void free_contact(Contact* contact);
void print_contacts(const AVLNode* root, char* output, unsigned int output_size);

#endif // HEADER_H