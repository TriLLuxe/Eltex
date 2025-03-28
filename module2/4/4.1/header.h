#ifndef HEADRE_H
#define HEADRE_H
#include <stddef.h>
// Структура для дополнительных данных контакта
typedef struct {
   char* phone;
   char* email;
   char* social_link;
   char* messenger_profile;
} ContactDetails;
// Структура для ФИО
typedef struct {
   char* last_name;
   char* first_name;
   char* middle_name;
} FullName;
// Основная структура контакта
typedef struct {
   FullName name;
   char* workplace;
   char* position;
   ContactDetails details;
} Contact;
// Структура узла двухсвязного списка
typedef struct Node {
   Contact contact;
   struct Node* next;
   struct Node* prev;
} Node;
// Функции для работы с двухсвязным списком
int add_contact(Node** head,
                const char* last_name, const char* first_name, const char* middle_name,
                const char* workplace, const char* position,
                const char* phone, const char* email,
                const char* social_link, const char* messenger_profile);
int edit_contact(Node** head, unsigned int id,
                 const char* last_name, const char* first_name, const char* middle_name,
                 const char* workplace, const char* position,
                 const char* phone, const char* email,
                 const char* social_link, const char* messenger_profile);
int delete_contact(Node** head, unsigned int id);
int save_to_file(const Node* head, const char* filename);
int load_from_file(Node** head, const char* filename);
void free_contact(Contact* contact);
void free_list(Node* head);
void print_contacts(const Node* head, char* output, unsigned int output_size);
#endif //HEADRE_H
