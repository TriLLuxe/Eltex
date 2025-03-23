#ifndef CONTACT_BOOK_H
#define CONTACT_BOOK_H

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


int add_contact(Contact** contacts, unsigned int* count, 
                const char* last_name, const char* first_name, const char* middle_name,
                const char* workplace, const char* position,
                const char* phone, const char* email, 
                const char* social_link, const char* messenger_profile);
int edit_contact(Contact* contacts, unsigned int count, unsigned int id,
                 const char* last_name, const char* first_name, const char* middle_name,
                 const char* workplace, const char* position,
                 const char* phone, const char* email, 
                 const char* social_link, const char* messenger_profile);
int delete_contact(Contact** contacts, unsigned int* count, unsigned int id);
int save_to_file(const Contact* contacts, unsigned int count, const char* filename);
int load_from_file(Contact** contacts, unsigned int* count, const char* filename);
void free_contact(Contact* contact);
void free_contacts(Contact* contacts, unsigned int count);
void print_contacts(const Contact* contacts, unsigned int count, char* output, unsigned int output_size);

#endif // CONTACT_BOOK_H    