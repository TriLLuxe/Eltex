#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    int sockfd;
    char sendline[1000], recvline[1000];
    struct sockaddr_in servaddr, cliaddr;

    if (argc != 2) {
        printf("Usage: a.out <IP address>\n");
        exit(1);
    }

    // Создаем сокет
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // Настраиваем адрес клиента
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    // Настраиваем адрес сервера
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    printf("Введите порт сервера: ");
    uint16_t port;
    scanf("%hu", &port);
    servaddr.sin_port = htons(port);
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
        printf("Invalid IP address\n");
        close(sockfd);
        exit(1);
    }

    // Регистрируемся на сервере
    strcpy(sendline, "hello");
    if (sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("sendto");
        close(sockfd);
        exit(1);
    }

    printf("Клиент запущен, вводите сообщения:\n");

    while (1) {
        printf("Вы: ");
        fgets(sendline, 1000, stdin);
        sendline[strcspn(sendline, "\n")] = 0;

        if (sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            perror("sendto");
            close(sockfd);
            exit(1);
        }

        int n = recvfrom(sockfd, recvline, 1000, 0, NULL, NULL);
        if (n < 0) {
            perror("recvfrom");
            close(sockfd);
            exit(1);
        }
        recvline[n] = '\0';
        printf("Другой клиент: %s\n", recvline);
    }

    close(sockfd);
    return 0;
}