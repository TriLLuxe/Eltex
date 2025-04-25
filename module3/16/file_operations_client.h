#ifndef FILE_OPERATIONS_CLIENT_H
#define FILE_OPERATIONS_CLIENT_H
#include <stdio.h>
void upload_file(int sock, char *buff, size_t buff_size);
void download_file(int sock, char *buff, size_t buff_size);

#endif