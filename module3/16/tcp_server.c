#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include "math_operations.h"
#include "file_operations.h"
#include "network_utils.h"

void dostuff(int);
void sigchld_handler(int sig);

int nclients = 0;

void printusers() {
    if (nclients) {
        printf("%d user on-line\n", nclients);
    } else {
        printf("No User on line\n");
    }
}

void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        nclients--;
        printf("-disconnect\n");
        printusers();
    }
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    int portno;
    int pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    signal(SIGCHLD, sigchld_handler);
    printf("TCP SERVER DEMO\n");

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        printf("usage %s port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[1]);
    if (portno <= 0 || portno > 65535) {
        fprintf(stderr, "ERROR, invalid port number\n");
        exit(1);
    }

    printf("Creating socket...\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        error("ERROR setting socket options");
    }

    printf("Binding socket...\n");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    printf("Listening on port %d...\n", portno);
    printf("IP: %s\n", inet_ntoa(serv_addr.sin_addr));
    if (listen(sockfd, 5) < 0) error("ERROR on listen");

    clilen = sizeof(cli_addr);

    while (1) {
        printf("Waiting for connection...\n");
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");
        nclients++;

        struct hostent *hst;
        hst = gethostbyaddr(&cli_addr.sin_addr, sizeof(cli_addr.sin_addr), AF_INET);
        char *client_ip = inet_ntoa(cli_addr.sin_addr);
        printf("+%s [%s] new connect!\n",
               hst ? hst->h_name : "Unknown host",
               client_ip ? client_ip : "Unknown IP");
        printusers();

        pid = fork();
        if (pid < 0) error("ERROR on fork");
        if (pid == 0) {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}

void dostuff(int sock) {
    int bytes_recv;
    double a, b;
    char buff[1024];
    const char *str1 = "Enter 1 parameter\r\n";
    const char *str2 = "Enter 2 parameter\r\n";
    const char *str3 = "Choose operation:\r\n1. ADD\r\n2. SUB\r\n3. MUL\r\n4. DIV\r\n5. QUIT\r\n6. UPLOAD FILE\r\n";

    while (1) {
        if (write(sock, str3, strlen(str3)) < 0) {
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
        errno = 0;
        long operation = strtol(buff, &endptr, 10);
        if (endptr == buff || *endptr != '\n' || errno != 0 || operation < 1 || operation > 6) {
            const char *err_msg = "ERROR: Invalid operation\n";
            if (write(sock, err_msg, strlen(err_msg)) < 0) {
                close(sock);
                return;
            }
            continue;
        }

        int status;
        if (operation >= 1 && operation <= 4) {
            if (write(sock, str1, strlen(str1)) < 0) {
                close(sock);
                return;
            }
            bytes_recv = read(sock, buff, sizeof(buff) - 1);
            if (bytes_recv <= 0) {
                close(sock);
                return;
            }
            buff[bytes_recv] = '\0';
            a = strtod(buff, &endptr);
            if (endptr == buff || (*endptr != '\n' && *endptr != '\0')) {
                const char *err_msg = "ERROR: Invalid first parameter\n";
                if (write(sock, err_msg, strlen(err_msg)) < 0) {
                    close(sock);
                    return;
                }
                continue;
            }

            if (write(sock, str2, strlen(str2)) < 0) {
                close(sock);
                return;
            }
            bytes_recv = read(sock, buff, sizeof(buff) - 1);
            if (bytes_recv <= 0) {
                close(sock);
                return;
            }
            buff[bytes_recv] = '\0';
            b = strtod(buff, &endptr);
            if (endptr == buff || (*endptr != '\n' && *endptr != '\0')) {
                const char *err_msg = "ERROR: Invalid second parameter\n";
                if (write(sock, err_msg, strlen(err_msg)) < 0) {
                close(sock);
                    return;
                }
                continue;
            }
        }

        switch (operation) {
            case 1:
                printf("ADD\n");
                a = add(a, b, &status);
                break;
            case 2:
                printf("SUB\n");
                a = sub(a, b, &status);
                break;
            case 3:
                printf("MUL\n");
                a = mul(a, b, &status);
                break;
            case 4:
                printf("DIV\n");
                a = _div(a, b, &status);
                break;
            case 5:
                printf("QUIT\n");
                close(sock);
                return;
            case 6:
                printf("UPLOAD FILE\n");
                upload_file(sock);
                continue;
            default:
                continue;
        }

        if (operation >= 1 && operation <= 4) {
            if (status == 1) {
                const char *err_msg = "ERROR: Division by zero\n";
                if (write(sock, err_msg, strlen(err_msg)) < 0) {
                    close(sock);
                    return;
                }
                continue;
            }
            snprintf(buff, sizeof(buff), "%.2f\n", a);
            if (write(sock, buff, strlen(buff)) < 0) {
                close(sock);
                return;
            }
        }
    }
}