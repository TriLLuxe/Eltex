#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void upload_file(int sock) {
    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        printf("Server disconnected\n");
        return;
    }
    buffer[n] = '\0';
    printf("S=>C: %s", buffer);

    // Ввод имени файла
    printf("C=>S: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("Input error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    char *filename = buffer;

    // Открытие файла
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("ERROR: Cannot open file %s\n", filename);
        return;
    }

    // Отправка имени файла
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        fclose(file);
        error("ERROR sending filename");
    }

    // Отправка содержимого файла
    while (!feof(file)) {
        int n = fread(buffer, 1, sizeof(buffer), file);
        if (send(sock, buffer, n, 0) < 0) {
            fclose(file);
            error("ERROR sending file data");
        }
    }

    // Отправка маркера конца файла
    if (send(sock, "EOF", 3, 0) < 0) {
        fclose(file);
        error("ERROR sending EOF");
    }
    fclose(file);

    // Получение подтверждения от сервера
    n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s", buffer);
    }
}

void download_file(int sock) {
    char buffer[1024];
    // Получение списка файлов и запроса имени файла
    while (1) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("Server disconnected\n");
            return;
        }
        buffer[n] = '\0';
        printf("S=>C: %s", buffer);
        if (strstr(buffer, "Enter filename for download") != NULL) {
            break;
        }
    }

    // Ввод имени файла
    printf("C=>S: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("Input error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    char *filename = buffer;

    // Отправка имени файла
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        error("ERROR sending filename");
    }

    // Создание файла для записи
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("ERROR: Cannot create file %s\n", filename);
        return;
    }

    // Получение данных файла
    while (1) {
        int n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            fclose(file);
            printf("Server disconnected or error occurred\n");
            return;
        }
        if (strncmp(buffer, "EOF", 3) == 0) {
            break; // Конец передачи
        }
        if (strncmp(buffer, "ERROR:", 6) == 0) {
            buffer[n] = '\0';
            printf("Server response: %s", buffer);
            fclose(file);
            return;
        }
        fwrite(buffer, 1, n, file);
    }
    fclose(file);
    printf("File %s downloaded successfully\n", filename);
}

int main(int argc, char *argv[]) {
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[1024];

    printf("TCP DEMO CLIENT\n");

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    if (portno <= 0 || portno > 65535) {
        fprintf(stderr, "ERROR, invalid port number\n");
        exit(1);
    }

    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0) error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(my_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    while (1) {
        n = recv(my_sock, buff, sizeof(buff) - 1, 0);
        if (n <= 0) {
            printf("Server disconnected\n");
            break;
        }
        buff[n] = '\0';
        printf("S=>C: %s", buff);

        printf("C=>S: ");
        if (fgets(buff, sizeof(buff), stdin) == NULL) {
            printf("Input error\n");
            break;
        }

        if (send(my_sock, buff, strlen(buff), 0) < 0) {
            error("ERROR sending to socket");
        }

        char *endptr;
        errno = 0;
        long operation = strtol(buff, &endptr, 10);
        if (endptr != buff && *endptr == '\n' && errno == 0 && operation >= 1 && operation <= 7) {
            if (operation == 5) {
                upload_file(my_sock);
            } else if (operation == 6) {
                download_file(my_sock);
            } else if (operation == 7) {
                printf("Exiting...\n");
                break;
            }
        }
    }

    close(my_sock);
    return 0;
}