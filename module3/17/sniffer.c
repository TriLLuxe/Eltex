#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#define SERVER_PORT 51000

#define MAX_PACKET_SIZE 65536

char *get_current_time() {
    static char buffer[20];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}   
int main(){
    int sockfd;
    char buffer[MAX_PACKET_SIZE];
    
    FILE* dump_file, *log_file;

    //Создание RAW сокета
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    //Создание файла для записи дампа и логов
    dump_file = fopen("dump.bin", "wb");
    if (dump_file == NULL) {
        perror("fopen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    log_file = fopen("log.txt", "w");
    if (log_file == NULL) {
        perror("fopen");
        fclose(dump_file);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Сниффер запущен. Захват UDP пакетов на порту %d\n", SERVER_PORT);
    fprintf(log_file, "Сниффер запущен. Захват UDP пакетов на порту %d\n", SERVER_PORT);
    
    while(1){
        ssize_t len = recv(sockfd, buffer, MAX_PACKET_SIZE, 0);
        if (len < 0 ){
            perror("recv");
            continue;
        }
        //Извлечение заголовка IP
        struct iphdr *ip_header = (struct iphdr *)buffer;
        if (ip_header->protocol != IPPROTO_UDP) {
            continue; // Пропустить, если не UDP пакет
        }
        //Извлечение заголовка UDP
        struct udphdr *udp_header = (struct udphdr *)(buffer + ip_header->ihl * 4);
        if (ntohs(udp_header->dest) != SERVER_PORT) {
            continue; // Пропустить, если не на целевой порт
        }
        //Вычисление начала данных
        int ip_header_len = ip_header->ihl * 4;
        int udp_header_len = sizeof(struct udphdr);
        int data_len = len - (ip_header_len + udp_header_len);
        char *data = buffer + ip_header_len + udp_header_len;

        if (data_len > 0 && data[data_len - 1] == '\0') {
            data[data_len - 1] = '\0'; 
            data_len--; 
        }

        fwrite(buffer, 1, len, dump_file);
        fflush(dump_file);
        char src_ip[INET_ADDRSTRLEN];
        char dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_header->saddr, src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &ip_header->daddr, dst_ip, INET_ADDRSTRLEN);
        printf("[%s] Пакет от %s:%d к %s:%d, данные: %s\n",
               get_current_time(),
               src_ip,
               ntohs(udp_header->source),
               dst_ip,
               ntohs(udp_header->dest),
               data);
        fprintf(log_file, "[%s] Пакет от %s:%d к %s:%d, данные: %s\n",
                get_current_time(),
                src_ip,
                ntohs(udp_header->source),
                dst_ip,
                ntohs(udp_header->dest),
                data);
        fflush(log_file);
    }
    fclose(dump_file);
    fclose(log_file);
    close(sockfd);
    return 0;
}