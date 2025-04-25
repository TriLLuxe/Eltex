#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
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
        printf("Debug: Ошибка отправки приглашения для загрузки\n");
        return;
    }
    int bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) {
        printf("Debug: Ошибка чтения имени файла для загрузки\n");
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
        printf("Debug: Ошибка отправки запроса размера файла\n");
        return;
    }
    bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) {
        printf("Debug: Ошибка чтения размера файла\n");
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
        printf("Debug: Ошибка отправки запроса содержимого файла\n");
        fclose(file);
        return;
    }
    size_t total_received = 0;
    while (total_received < file_size) {
        bytes_recv = read(sock, buff, sizeof(buff));
        if (bytes_recv <= 0) {
            printf("Debug: Ошибка чтения содержимого файла\n");
            fclose(file);
            return;
        }
        fwrite(buff, 1, bytes_recv, file);
        total_received += bytes_recv;
    }
    fclose(file);

    const char *success_msg = "File uploaded successfully\n";
    write(sock, success_msg, strlen(success_msg));
}

void download_file(int sock) {
    char buff[1024];
    char filename[256];
    FILE *file;

    printf("Debug: Начало download_file\n");
    const char *prompt_download = "Enter filename to download:\n";
    if (write(sock, prompt_download, strlen(prompt_download)) < 0) {
        printf("Debug: Ошибка отправки приглашения для скачивания\n");
        return;
    }
    // Чтение команды DOWNLOAD от клиента
    int bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) {
        printf("Debug: Ошибка чтения команды DOWNLOAD\n");
        return;
    }
    buff[bytes_recv] = '\0';
    printf("Debug: Получена команда: '%s'\n", buff);

    // Проверка, начинается ли команда с "DOWNLOAD "
    if (strncmp(buff, "DOWNLOAD ", 9) != 0) {
        const char *err_msg = "ERROR: Invalid command\n";
        write(sock, err_msg, strlen(err_msg));
        printf("Debug: Неверная команда\n");
        return;
    }

    // Извлечение имени файла
    strncpy(filename, buff + 9, sizeof(filename) - 1);
    filename[sizeof(filename) - 1] = '\0';
    filename[strcspn(filename, "\n")] = '\0';
    printf("Debug: Имя файла: '%s'\n", filename);

    // Формирование полного пути
    char prefixed_filename[512];
    snprintf(prefixed_filename, sizeof(prefixed_filename), "files/%s", filename);

    // Попытка открыть файл
    file = fopen(prefixed_filename, "rb");
    if (file == NULL) {
        const char *err_msg = "ERROR: Cannot open file\n";
        write(sock, err_msg, strlen(err_msg));
        printf("Debug: Не удалось открыть файл\n");
        return;
    }

    // Получение размера файла
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("Debug: Размер файла: %ld\n", file_size);

    // Отправка ответа SIZE
    snprintf(buff, sizeof(buff), "SIZE %ld\n", file_size);
    if (write(sock, buff, strlen(buff)) < 0) {
        fclose(file);
        printf("Debug: Ошибка отправки SIZE\n");
        return;
    }

    // Ожидание READY от клиента
    bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) {
        fclose(file);
        printf("Debug: Ошибка чтения READY\n");
        return;
    }
    buff[bytes_recv] = '\0';
    printf("Debug: Получено: '%s'\n", buff);
    if (strcmp(buff, "READY\n") != 0) {
        fclose(file);
        printf("Debug: Не получено READY\n");
        return;
    }

    // Отправка содержимого файла
    size_t total_sent = 0;
    while (total_sent < file_size) {
        size_t to_read = file_size - total_sent;
        if (to_read > sizeof(buff)) to_read = sizeof(buff);
        size_t bytes_read = fread(buff, 1, to_read, file);
        if (bytes_read == 0) break;
        if (write(sock, buff, bytes_read) < 0) {
            fclose(file);
            printf("Debug: Ошибка отправки содержимого файла\n");
            return;
        }
        total_sent += bytes_read;
    }
    fclose(file);
    printf("Debug: Файл отправлен, всего байт: %zu\n", total_sent);

    // Отправка DONE
    const char *done_msg = "DONE\n";
    if (write(sock, done_msg, strlen(done_msg)) < 0) {
        printf("Debug: Ошибка отправки DONE\n");
        return;
    }
    printf("Debug: Завершение download_file\n");
}