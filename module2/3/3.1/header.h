#ifndef HEADER_H
#define HEADER_H

#include <sys/stat.h>

void letter_to_bits(const char *letters, char *bits);
void number_to_bits(const char *number, char *bits);
void bits_to_letters(const char *bits, char *letters);
void bits_to_number(const char *bits, char *number);
int get_file_permissions(const char *filename, char *bits);
void apply_modification(const char *command, char *bits);

#endif