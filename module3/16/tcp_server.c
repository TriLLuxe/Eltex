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
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

void dostuff(int);
void error(const char *msg) {
    perror(msg);
    exit(1);
}

int nclients = 0;

void printusers() {
    if (nclients) {
        printf("%d user on-line\n", nclients);
    } else {
        printf("No User on line\n");
    }
}

double add(double a, double b, int *status) {
    *status = 0;
    return a + b;
}

double sub(double a, double b, int *status) {
    *status = 0;
    return a - b;
}

double mul(double a, double b, int *status) {
    *status = 0;
    return a * b;
}

double _div(double a, double b, int *status) {
    if (b == 0) {
        fprintf(stderr, "ERROR: Division by zero\n");
        *status = 1;
        return 0;
    }
    *status = 0;
    return a / b;
}

void list_files(int sock) {
    DIR *dir;
    struct dirent *entry;
    char buff[1024];
    dir = opendir("files");
    if (dir == NULL) {
        perror("ERROR opening directory files/");
        if (sock != -1) {
            snprintf(buff, sizeof(buff), "ERROR: Cannot open directory files/\r\n");
            write(sock, buff, strlen(buff));
        }
        return;
    }
    printf("Files in directory 'files/':\n");
    if (sock != -1) {
        snprintf(buff, sizeof(buff), "Available files:\r\n");
        write(sock, buff, strlen(buff));
    }
    while ((entry = readdir(dir)) != NULL) {
        char filepath[2048];
        snprintf(filepath, sizeof(filepath), "files/%s", entry->d_name);
        struct stat file_stat;
        if (stat(filepath, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) { 
            printf("- %s\n", entry->d_name);
            if (sock != -1) {
                snprintf(buff, sizeof(buff), "- %s\r\n", entry->d_name);
                write(sock, buff, strlen(buff));
            }
        }
    }
    if (sock != -1) {
        snprintf(buff, sizeof(buff), "End of file list\r\n");
        write(sock, buff, strlen(buff));
    }
    closedir(dir);
}

void upload_file(int sock, int *status) {
    char buff[1024];
    const char *str_file = "Enter filename for upload\r\n";
    *status = 0;
    if (write(sock, str_file, strlen(str_file)) < 0) {
        *status = 2;
        return;
    }
    int bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) {
        *status = 2;
        return;
    }
    buff[bytes_recv] = '\0';
    char *filename = buff;
    filename[strcspn(filename, "\n")] = '\0';

    // Create path in files/ directory
    char filepath[2048];
    snprintf(filepath, sizeof(filepath), "files/%s", filename);
    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        const char *err_msg = "ERROR: Cannot create file\n";
        write(sock, err_msg, strlen(err_msg));
        *status = 2;
        return;
    }
    while (1) {
        bytes_recv = read(sock, buff, sizeof(buff));
        if (bytes_recv <= 0) {
            fclose(file);
            *status = 2;
            return;
        }
        if (strncmp(buff, "EOF", 3) == 0) {
            break;
        }
        fwrite(buff, 1, bytes_recv, file);
    }
    fclose(file);
    const char *success_msg = "File uploaded successfully\n";
    if (write(sock, success_msg, strlen(success_msg)) < 0) {
        *status = 2;
        return;
    }
    // List files in directory
    list_files(-1); // Only print to server console
}

void download_file(int sock, int *status) {
    char buff[1024];
    const char *str_file = "Enter filename for download\r\n";
    *status = 0;
    // Send list of files to client
    list_files(sock);
    // Request filename
    if (write(sock, str_file, strlen(str_file)) < 0) {
        *status = 3;
        return;
    }
    int bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) {
        *status = 3;
        return;
    }
    buff[bytes_recv] = '\0';
    char *filename = buff;
    filename[strcspn(filename, "\n")] = '\0';

    // Create path in files/ directory
    char filepath[2048];
    snprintf(filepath, sizeof(filepath), "files/%s", filename);
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        const char *err_msg = "ERROR: File not found\n";
        write(sock, err_msg, strlen(err_msg));
        *status = 3;
        return;
    }
    while (!feof(file)) {
        int n = fread(buff, 1, sizeof(buff), file);
        if (n > 0) {
            if (write(sock, buff, n) < 0) {
                fclose(file);
                *status = 3;
                return;
            }
        }
    }
    fclose(file);
    if (write(sock, "EOF", 3) < 0) {
        *status = 3;
    }
}

void disconnect(int sock) {
    close(sock);
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

    // Create files/ directory if it doesn't exist
    mkdir("files", 0755);

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
    const char *str3 = "Choose operation:\r\n1. ADD\r\n2. SUB\r\n3. MUL\r\n4. DIV\r\n5. UPLOAD\r\n6. DOWNLOAD\r\n7. QUIT\r\n";

    while (1) {
        if (write(sock, str3, strlen(str3)) < 0) {
            disconnect(sock);
            return;
        }

        bytes_recv = read(sock, buff, sizeof(buff) - 1);
        if (bytes_recv <= 0) {
            disconnect(sock);
            return;
        }
        buff[bytes_recv] = '\0';

        char *endptr;
        errno = 0;
        long operation = strtol(buff, &endptr, 10);
        if (endptr == buff || *endptr != '\n' || errno != 0 || operation < 1 || operation > 7) {
            const char *err_msg = "ERROR: Invalid operation\n";
            if (write(sock, err_msg, strlen(err_msg)) < 0) {
                disconnect(sock);
                return;
            }
            continue;
        }

        int status;
        if (operation >= 1 && operation <= 4) {
            const char *str1 = "Enter 1 parameter\r\n";
            if (write(sock, str1, strlen(str1)) < 0) {
                disconnect(sock);
                return;
            }
            bytes_recv = read(sock, buff, sizeof(buff) - 1);
            if (bytes_recv <= 0) {
                disconnect(sock);
                return;
            }
            buff[bytes_recv] = '\0';
            a = strtod(buff, &endptr);
            if (endptr == buff || (*endptr != '\n' && *endptr != '\0')) {
                const char *err_msg = "ERROR: Invalid first parameter\n";
                if (write(sock, err_msg, strlen(err_msg)) < 0) {
                    disconnect(sock);
                    return;
                }
                continue;
            }

            const char *str2 = "Enter 2 parameter\r\n";
            if (write(sock, str2, strlen(str2)) < 0) {
                disconnect(sock);
                return;
            }
            bytes_recv = read(sock, buff, sizeof(buff) - 1);
            if (bytes_recv <= 0) {
                disconnect(sock);
                return;
            }
            buff[bytes_recv] = '\0';
            b = strtod(buff, &endptr);
            if (endptr == buff || (*endptr != '\n' && *endptr != '\0')) {
                const char *err_msg = "ERROR: Invalid second parameter\n";
                if (write(sock, err_msg, strlen(err_msg)) < 0) {
                    disconnect(sock);
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
                printf("UPLOAD\n");
                upload_file(sock, &status);
                break;
            case 6:
                printf("DOWNLOAD\n");
                download_file(sock, &status);
                break;
            case 7:
                printf("QUIT\n");
                disconnect(sock);
                return;
            default:
                continue;
        }

        if (status == 1) {
            const char *err_msg = "ERROR: Division by zero\n";
            if (write(sock, err_msg, strlen(err_msg)) < 0) {
                disconnect(sock);
                return;
            }
            continue;
        } else if (status == 2) {
            const char *err_msg = "ERROR: File uploading failed\n";
            if (write(sock, err_msg, strlen(err_msg)) < 0) {
                disconnect(sock);
                return;
            }
            continue;
        } else if (status == 3) {
            const char *err_msg = "ERROR: File downloading failed\n";
            if (write(sock, err_msg, strlen(err_msg)) < 0) {
                disconnect(sock);
                return;
            }
            continue;
        }

        if (operation >= 1 && operation <= 4) {
            snprintf(buff, sizeof(buff), "%.2f\n", a);
            if (write(sock, buff, strlen(buff)) < 0) {
                disconnect(sock);
                return;
            }
        }
    }
}