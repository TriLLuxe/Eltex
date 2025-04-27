#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define SERVER_PORT 51000

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int nclients = 0;

void printusers() {
    if (nclients) {
        printf("%d user(s) on-line\n", nclients);
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

typedef enum {
    STATE_WAIT_OP,
    STATE_WAIT_A,
    STATE_WAIT_B,
}client_state;

typedef struct {
    int sock; //Дескриптор сокета
    client_state state;//Состояние клиента
    double a, b; //Операнды
    char buffer[BUFFER_SIZE]; //Буфер для хранения данных
    int buffer_pos; //Текущая позиция в буфере
    int operation; //Операция
}client;

int main(int argc, char *argv[]) {
    int sockfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
    client clients[MAX_CLIENTS];
    fd_set read_fds;
    int max_fd;
    int i;

    // Инициализация массива клиентов
    for (i = 0; i < MAX_CLIENTS; i++) {
        clients[i].sock = -1;
        clients[i].state = STATE_WAIT_OP;
        clients[i].buffer_pos = 0;
        clients[i].operation = 0;
    }

    printf("TCP DEMO SERVER\n");
    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
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
    printf("Waiting for connection...\n");

    while(1){
        //Подготовка к select
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        max_fd = sockfd;

        //Добавляем активные сокеты клиентов в набор
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].sock != -1) {
                FD_SET(clients[i].sock, &read_fds);
                if (clients[i].sock > max_fd) {
                    max_fd = clients[i].sock;
                }
            }
        }

        //Ожидание активности на сокетах
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            if(errno == EINTR) continue; // Игнорируем прерывание 
            error("ERROR on select");
        }

        //Проверяем сокет клиента на готовность
        if (FD_ISSET(sockfd, &read_fds)) {
            socklen_t clilen = sizeof(cli_addr);
            int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) error("ERROR on accept");
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].sock == -1) {
                    clients[i].sock = newsockfd;
                    clients[i].state = STATE_WAIT_OP;
                    clients[i].buffer_pos = 0;
                    nclients++;
                    break;
                }
            }
            printf("New connection from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            printusers();
            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Max clients reached. Connection refused.\n");
                close(newsockfd);
            } else{
                /*struct hostent *hst = gethostbyaddr((const char *)&cli_addr.sin_addr, sizeof(cli_addr.sin_addr), AF_INET);
                char *client_ip = inet_ntoa(cli_addr.sin_addr);
                printf("+%s [%s] new connect!\n",
                       hst ? hst->h_name : "Unknown host",
                       client_ip ? client_ip : "Unknown IP");
                printusers();*/

                // Отправляем меню операций
                const char *str3 = "Choose operation:\r\n1. ADD\r\n2. SUB\r\n3. MUL\r\n4. DIV\r\n5. QUIT\r\n";
                if (write(newsockfd, str3, strlen(str3)) < 0) {
                    close(newsockfd);
                    clients[i].sock = -1;   
                    nclients--;
                    printf("-disconnect\n");
                    printusers();
                }
            }
        }

    // Проверяем данные от клиента
    for (i = 0; i < MAX_CLIENTS;i++){
        if (clients[i].sock == -1 || !FD_ISSET(clients[i].sock, &read_fds)) continue;
        int bytes_recv = recv(clients[i].sock, clients[i].buffer + clients[i].buffer_pos, BUFFER_SIZE - clients[i].buffer_pos - 1, 0);
        if (bytes_recv <= 0) {
            printf("-disconnect\n");
            close(clients[i].sock);
            clients[i].sock = -1;
            nclients--;
            printusers();
            continue;
        }
        clients[i].buffer_pos += bytes_recv;
        clients[i].buffer[clients[i].buffer_pos] = '\0';
        // Проверяем наличие символа новой строки
        char *newline = strchr(clients[i].buffer, '\n');
            if (!newline) continue;
        *newline = '\0';
        char *input = clients[i].buffer;
        char *endptr;
        int status;

        switch (clients[i].state) {
            case STATE_WAIT_OP: {
                errno = 0;
                long operation = strtol(input, &endptr, 10);
                if (endptr == input || *endptr != '\0' || errno != 0 || operation < 1 || operation > 5) {
                    const char *err_msg = "ERROR: Invalid operation\n";
                    write(clients[i].sock, err_msg, strlen(err_msg));
                    write(clients[i].sock, "Choose operation:\r\n1. ADD\r\n2. SUB\r\n3. MUL\r\n4. DIV\r\n5. QUIT\r\n",
                          strlen("Choose operation:\r\n1. ADD\r\n2. SUB\r\n3. MUL\r\n4. DIV\r\n5. QUIT\r\n"));
                    clients[i].buffer_pos = 0;
                    continue;
                }

                if (operation == 5) {
                    printf("QUIT\n");
                    close(clients[i].sock);
                    clients[i].sock = -1;
                    nclients--;
                    printf("-disconnect\n");
                    printusers();
                    continue;
                }

                clients[i].state = STATE_WAIT_A;
                write(clients[i].sock, "Enter 1 parameter\r\n", strlen("Enter 1 parameter\r\n"));
                clients[i].buffer_pos = 0;
                clients[i].operation = operation; 
                break;
            }

            case STATE_WAIT_A: {
                double a = strtod(input, &endptr);
                if (endptr == input || *endptr != '\0') {
                    const char *err_msg = "ERROR: Invalid first parameter\n";
                    write(clients[i].sock, err_msg, strlen(err_msg));
                    write(clients[i].sock, "Enter 1 parameter\r\n", strlen("Enter 1 parameter\r\n"));
                    clients[i].buffer_pos = 0;
                    continue;
                }

                clients[i].a = a;
                clients[i].state = STATE_WAIT_B;
                write(clients[i].sock, "Enter 2 parameter\r\n", strlen("Enter 2 parameter\r\n"));
                clients[i].buffer_pos = 0;
                break;
            }

            case STATE_WAIT_B: {
                double b = strtod(input, &endptr);
                if (endptr == input || *endptr != '\0') {
                    const char *err_msg = "ERROR: Invalid second parameter\n";
                    write(clients[i].sock, err_msg, strlen(err_msg));
                    write(clients[i].sock, "Enter 2 parameter\r\n", strlen("Enter 2 parameter\r\n"));
                    clients[i].buffer_pos = 0;
                    continue;
                }

                clients[i].b = b;
                
                double result;
                switch (clients[i].operation) { 
                    case 1:
                        printf("ADD\n");
                        result = add(clients[i].a, b, &status);
                        break;
                    case 2:
                        printf("SUB\n");
                        result = sub(clients[i].a, b, &status);
                        break;
                    case 3:
                        printf("MUL\n");
                        result = mul(clients[i].a, b, &status);
                        break;
                    case 4:
                        printf("DIV\n");
                        result = _div(clients[i].a, b, &status);
                        break;
                    default:
                        status = 1;
                        break;
                }

                if (status == 1) {
                    const char *err_msg = "ERROR: Division by zero\n";
                    write(clients[i].sock, err_msg, strlen(err_msg));
                } else {
                    char result_buf[32];
                    snprintf(result_buf, sizeof(result_buf), "%.2f\n", result);
                    write(clients[i].sock, result_buf, strlen(result_buf));
                }

                clients[i].state = STATE_WAIT_OP;
                write(clients[i].sock, "Choose operation:\r\n1. ADD\r\n2. SUB\r\n3. MUL\r\n4. DIV\r\n5. QUIT\r\n",
                      strlen("Choose operation:\r\n1. ADD\r\n2. SUB\r\n3. MUL\r\n4. DIV\r\n5. QUIT\r\n"));
                clients[i].buffer_pos = 0;
                break;
            }
        }
     }
        
    }
    close(sockfd);
    return 0;
}