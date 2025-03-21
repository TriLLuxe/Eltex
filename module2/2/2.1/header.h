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
    char* last_name;   // (обязательное поле)
    char* first_name;  // (обязательное поле)
    char* middle_name; // (опционально)
} FullName;

// Основная структура контакта
typedef struct {
    FullName name;
    char* workplace;
    char* position;
    ContactDetails details;
} Contact;


void add_contact(Contact** contacts, unsigned int* count);
void edit_contact(Contact* contacts, unsigned int count, unsigned int id);
void delete_contact(Contact** contacts, unsigned int* count, unsigned int id);
void save_to_file(const Contact* contacts, unsigned int count, const char* filename);
void load_from_file(Contact** contacts, unsigned int* count, const char* filename);
void free_contact(Contact* contact);
void free_contacts(Contact* contacts, unsigned int count);
void print_contacts(const Contact* contacts, unsigned int count);
#endif // CONTACT_BOOK_H