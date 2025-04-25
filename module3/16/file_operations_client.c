#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "file_operations_client.h"
#include "network_utils.h"

void upload_file(int sock, char *buff, size_t buff_size) {
    char *current_filename = NULL;
    FILE *current_file = NULL;
    long current_file_size = 0;

    // Запрашиваем имя файла
    printf("C=>S: ");
    if (fgets(buff, buff_size, stdin) == NULL) {
        printf("Input error\n");
        return;
    }
    current_filename = strdup(buff);
    if (current_filename == NULL) {
        error("Memory allocation error");
    }
    current_filename[strcspn(current_filename, "\n")] = 0;
    current_file = fopen(current_filename, "rb");
    if (current_file == NULL) {
        const char *err_msg = "ERROR: Cannot open file\n";
        if (send(sock, err_msg, strlen(err_msg), 0) < 0) {
            error("ERROR sending to socket");
        }
        free(current_filename);
        return;
    }
    fseek(current_file, 0, SEEK_END);
    current_file_size = ftell(current_file);
    fseek(current_file, 0, SEEK_SET);
    if (send(sock, buff, strlen(buff), 0) < 0) {
        error("ERROR sending to socket");
    }

    // Ожидаем запрос на размер файла
    int n = recv(sock, buff, buff_size - 1, 0);
    if (n <= 0) {
        printf("Server disconnected\n");
        fclose(current_file);
        free(current_filename);
        return;
    }
    buff[n] = '\0';
    printf("S=>C: %s", buff);
    if (strcmp(buff, "Enter file size:\n") != 0) {
        fclose(current_file);
        free(current_filename);
        return;
    }

    // Отправляем размер файла
    snprintf(buff, buff_size, "%ld\n", current_file_size);
    if (send(sock, buff, strlen(buff), 0) < 0) {
        error("ERROR sending to socket");
    }

    // Ожидаем запрос на содержимое файла
    n = recv(sock, buff, buff_size - 1, 0);
    if (n <= 0) {
        printf("Server disconnected\n");
        fclose(current_file);
        free(current_filename);
        return;
    }
    buff[n] = '\0';
    printf("S=>C: %s", buff);
    if (strcmp(buff, "Send file content:\n") != 0) {
        fclose(current_file);
        free(current_filename);
        return;
    }

    // Отправляем содержимое файла
    size_t total_sent = 0;
    while (total_sent < current_file_size) {
        size_t to_send = current_file_size - total_sent;
        if (to_send > buff_size) to_send = buff_size;
        size_t bytes_read = fread(buff, 1, to_send, current_file);
        if (bytes_read == 0) break;
        if (send(sock, buff, bytes_read, 0) < 0) {
            error("ERROR sending to socket");
        }
        total_sent += bytes_read;
    }
    fclose(current_file);
    free(current_filename);

    // Ожидаем подтверждение успешной загрузки
    n = recv(sock, buff, buff_size - 1, 0);
    if (n <= 0) {
        printf("Server disconnected\n");
        return;
    }
    buff[n] = '\0';
    printf("S=>C: %s", buff);
}