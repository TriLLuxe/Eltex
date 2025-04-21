#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX_CLIENTS 2

int main() {
    int sockfd;
    char line[1000];
    struct sockaddr_in servaddr, cliaddr[MAX_CLIENTS];/* Структуры для адресов сервера и клиентов */
    int client_count = 0; 

    
    /* Заполняем структуру для адреса сервера */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; /* IPv4 */
    servaddr.sin_port = htons(51000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    
    //создание сокета UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    // Настраиваем адрес сокета
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }
    printf("Сервер запущен,IP адрес: %s, порт: %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
    printf("Ожидание клиентов...\n");
    
    while(1){
        //Основной цикл обслуживания клиентов
        struct sockaddr_in sender_addr;
        socklen_t sender_len = sizeof(sender_addr);
        int n = recvfrom(sockfd, line, 999, 0, (struct sockaddr *)&sender_addr, &sender_len);
        if (n < 0) {
            perror("recvfrom");
            close(sockfd);
            exit(1);
        }
        line[n] = '\0'; 
        // Проверяем, зарегистрирован ли клиент
        int registered = 0;
        for (int i = 0; i < client_count; i++) {
            if (memcmp(&cliaddr[i], &sender_addr, sizeof(sender_addr)) == 0) {
                registered = 1;
                break;
            }
        }
        // Регистрируем нового клиента
        if (!registered && client_count < MAX_CLIENTS){
            memcpy( &cliaddr[client_count], &sender_addr, sizeof(sender_addr));
            client_count++;
            printf("Клиент %s:%d зарегистрирован\n", inet_ntoa(sender_addr.sin_addr), ntohs(sender_addr.sin_port));
            continue;
        }
        // отправляем ответ клиенту
        for (int i = 0; i < client_count; i++) {
            if (memcmp(&cliaddr[i], &sender_addr, sizeof(sender_addr)) != 0){
                if (sendto(sockfd, line, strlen(line) + 1, 0, (struct sockaddr *)&cliaddr[i], sizeof(cliaddr[i])) < 0) {
                    perror("sendto");
                    close(sockfd);
                    exit(1);
                }
                printf("Отправлено клиенту %s:%d сообщение: %s\n", inet_ntoa(cliaddr[i].sin_addr), ntohs(cliaddr[i].sin_port), line);
            }
        }
    }
    close(sockfd);
    return 0;
}  
