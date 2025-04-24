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
        if (strncmp(buff, "Division by zero", 15) == 0) continue;;
        printf("C=>S: ");
        if (fgets(buff, sizeof(buff), stdin) == NULL) {
            printf("Input error\n");
            break;
        }

        if (send(my_sock, buff, strlen(buff), 0) < 0) {
            error("ERROR sending to socket");
        }
    }

    close(my_sock);
    return 0;
}