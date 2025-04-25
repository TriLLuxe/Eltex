#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include "file_operations_client.h"
#include "network_utils.h"
#include <stdint.h>

void upload_file(int sock, char *buff, size_t buff_size) {
    char *current_filename = NULL;
    FILE *current_file = NULL;
    long current_file_size = 0;

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

    snprintf(buff, buff_size, "%ld\n", current_file_size);
    if (send(sock, buff, strlen(buff), 0) < 0) {
        error("ERROR sending to socket");
    }

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

    n = recv(sock, buff, buff_size - 1, 0);
    if (n <= 0) {
        printf("Server disconnected\n");
        return;
    }
    buff[n] = '\0';
    printf("S=>C: %s", buff);
}

void download_file(int sock, char *buff, size_t buff_size) {
    printf("C=>S: ");
    if (fgets(buff, buff_size - 10, stdin) == NULL) {
        printf("Input error\n");
        return;
    }
    char *filename = strdup(buff);
    if (filename == NULL) {
        error("Memory allocation error");
    }
    filename[strcspn(filename, "\n")] = '\0';
    snprintf(buff, buff_size, "DOWNLOAD %s\n", filename);
    if (send(sock, buff, strlen(buff), 0) < 0) {
        free(filename);
        error("ERROR sending to socket");
    }

    int n = recv(sock, buff, buff_size - 1, 0);
    if (n <= 0) {
        printf("Server disconnected\n");
        free(filename);
        return;
    }
    buff[n] = '\0';
    printf("S=>C: %s", buff);

    if (strncmp(buff, "ERROR", 5) == 0) {
        free(filename);
        return;
    }

    if (strncmp(buff, "SIZE ", 5) != 0) {
        printf("Invalid response from server\n");
        free(filename);
        return;
    }
    char *endptr;
    long file_size = strtol(buff + 5, &endptr, 10);
    if (endptr == buff + 5 || *endptr != '\n') {
        printf("Invalid size response from server\n");
        free(filename);
        return;
    }

    const char *ready_msg = "READY\n";
    if (send(sock, ready_msg, strlen(ready_msg), 0) < 0) {
        free(filename);
        error("ERROR sending to socket");
    }

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Cannot open file for writing\n");
        free(filename);
        return;
    }

    size_t total_received = 0;
    while (total_received < file_size) {
        size_t to_receive = file_size - total_received;
        if (to_receive > buff_size) to_receive = buff_size;
        int bytes_received = recv(sock, buff, to_receive, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            fclose(file);
            free(filename);
            return;
        }
        fwrite(buff, 1, bytes_received, file);
        total_received += bytes_received;
    }
    fclose(file);

    n = recv(sock, buff, buff_size - 1, 0);
    if (n <= 0) {
        printf("Server disconnected\n");
        free(filename);
        return;
    }
    buff[n] = '\0';

    if (strncmp(buff, "DONE\n", 5) == 0) {
        printf("File downloaded successfully\n");
        if (n > 5) {
            memmove(buff, buff + 5, n - 5);
            buff[n - 5] = '\0';
            printf("S=>C: %s", buff);
            printf("C=>S: ");
            if (fgets(buff, buff_size, stdin) == NULL) {
                printf("Input error\n");
                free(filename);
                return;
            }
            if (send(sock, buff, strlen(buff), 0) < 0) {
                error("ERROR sending to socket");
            }
        }
    } else {
        printf("Unexpected response: '%s'\n", buff);
    }
    free(filename);
}