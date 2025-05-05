#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>

// Структура для хранения информации о клиенте
typedef struct {
    struct sockaddr_in addr;
    uint32_t msg_count;
} Client;

//Хеш-таблица для хранения клиентов
#define MAX_CLIENTS 100
Client clients[MAX_CLIENTS];
int client_count = 0;
//Поиск клиента в хеш-таблице
Client* find_client(struct sockaddr_in *addr){
    for (int i = 0; i < client_count; ++i){
        if (clients[i].addr.sin_addr.s_addr == addr->sin_addr.s_addr &&
            clients[i].addr.sin_port == addr->sin_port) 
            return &clients[i];
    }
    return NULL;
}

//Добавление нового клиента 
Client *add_client(struct sockaddr_in *addr){
    if (client_count >= MAX_CLIENTS){
        fprintf(stderr, "Max clients reached\n");
        return NULL;
    }
    clients[client_count].addr = *addr;
    clients[client_count].msg_count = 0;
    return &clients[client_count++];
}

//Флаг для обработки сигналов
volatile sig_atomic_t running = 1;

//Обработчик сигнала
void signal_handler(int sig){
    running = 0;
}

int main (int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535){
        fprintf(stderr, "Invalid port number\n");
        exit(EXIT_FAILURE);
    }
    //Установка обработчика сигнала
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    //создание raw сокета
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) < 0){
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    char buffer[65536];
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    
}