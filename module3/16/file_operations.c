#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "file_operations.h"

void upload_file(int sock) {
    char buff[1024];
    char filename[256];
    long file_size;
    FILE *file;

    const char *prompt_filename = "Enter filename to upload:\n";
    if (write(sock, prompt_filename, strlen(prompt_filename)) < 0) {
        close(sock);
        return;
    }
    int bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) {
        close(sock);
        return;
    }
    buff[bytes_recv] = '\0';
    strncpy(filename, buff, sizeof(filename) - 1);
    char prefixed_filename[512]; 
    filename[sizeof(filename) - 1] = '\0';
    filename[strcspn(filename, "\n")] = '\0';
    snprintf(prefixed_filename, sizeof(prefixed_filename), "files/%s", filename);

    const char *prompt_size = "Enter file size:\n";
    if (write(sock, prompt_size, strlen(prompt_size)) < 0) {
        close(sock);
        return;
    }
    bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) {
        close(sock);
        return;
    }
    buff[bytes_recv] = '\0';
    char *endptr;
    file_size = strtol(buff, &endptr, 10);
    if (endptr == buff || *endptr != '\n' || file_size <= 0) {
        const char *err_msg = "ERROR: Invalid file size\n";
        write(sock, err_msg, strlen(err_msg));
        return;
    }

    file = fopen(prefixed_filename, "wb");
    if (file == NULL) {
        const char *err_msg = "ERROR: Cannot open file for writing\n";
        write(sock, err_msg, strlen(err_msg));
        return;
    }

    const char *prompt_content = "Send file content:\n";
    if (write(sock, prompt_content, strlen(prompt_content)) < 0) {
        fclose(file);
        close(sock);
        return;
    }
    size_t total_received = 0;
    while (total_received < file_size) {
        bytes_recv = read(sock, buff, sizeof(buff));
        if (bytes_recv <= 0) {
            fclose(file);
            close(sock);
            return;
        }
        fwrite(buff, 1, bytes_recv, file);
        total_received += bytes_recv;
    }
    fclose(file);

    const char *success_msg = "File uploaded successfully\n";
    write(sock, success_msg, strlen(success_msg));
}