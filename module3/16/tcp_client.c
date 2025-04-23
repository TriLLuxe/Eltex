#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[1024];
    char *current_filename = NULL;
    FILE *current_file = NULL;
    long current_file_size = 0;

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

        if (strcmp(buff, "Enter filename to upload:\n") == 0) {
            printf("C=>S: ");
            if (fgets(buff, sizeof(buff), stdin) == NULL) {
                printf("Input error\n");
                break;
            }
            current_filename = strdup(buff);
            if (current_filename == NULL) {
                error("Memory allocation error");
            }
            current_filename[strcspn(current_filename, "\n")] = 0;
            current_file = fopen(current_filename, "rb");
            if (current_file == NULL) {
                const char *err_msg = "ERROR: Cannot open file\n";
                if (send(my_sock, err_msg, strlen(err_msg), 0) < 0) {
                    error("ERROR sending to socket");
                }
                free(current_filename);
                current_filename = NULL;
                continue;
            }
            fseek(current_file, 0, SEEK_END);
            current_file_size = ftell(current_file);
            fseek(current_file, 0, SEEK_SET);
            if (send(my_sock, buff, strlen(buff), 0) < 0) {
                error("ERROR sending to socket");
            }
        } else if (strcmp(buff, "Enter file size:\n") == 0) {
            if (current_file == NULL) {
                const char *err_msg = "ERROR: No file opened\n";
                if (send(my_sock, err_msg, strlen(err_msg), 0) < 0) {
                    error("ERROR sending to socket");
                }
                continue;
            }
            snprintf(buff, sizeof(buff), "%ld\n", current_file_size);
            if (send(my_sock, buff, strlen(buff), 0) < 0) {
                error("ERROR sending to socket");
            }
        } else if (strcmp(buff, "Send file content:\n") == 0) {
            if (current_file == NULL) {
                const char *err_msg = "ERROR: No file opened\n";
                if (send(my_sock, err_msg, strlen(err_msg), 0) < 0) {
                    error("ERROR sending to socket");
                }
                continue;
            }
            size_t total_sent = 0;
            while (total_sent < current_file_size) {
                size_t to_send = current_file_size - total_sent;
                if (to_send > sizeof(buff)) to_send = sizeof(buff);
                size_t bytes_read = fread(buff, 1, to_send, current_file);
                if (bytes_read == 0) break;
                if (send(my_sock, buff, bytes_read, 0) < 0) {
                    error("ERROR sending to socket");
                }
                total_sent += bytes_read;
            }
            fclose(current_file);
            current_file = NULL;
            free(current_filename);
            current_filename = NULL;
        } else {
            printf("C=>S: ");
            if (fgets(buff, sizeof(buff), stdin) == NULL) {
                printf("Input error\n");
                break;
            }
            if (send(my_sock, buff, strlen(buff), 0) < 0) {
                error("ERROR sending to socket");
            }
            if (strcmp(buff, "5\n") == 0) {
                printf("Exiting...\n");
                break;
            }
        }
    }

    close(my_sock);
    return 0;
}