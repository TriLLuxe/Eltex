#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include "header.h"

int sockfd;
struct sockaddr_in server_addr;
volatile sig_atomic_t running = 1;

// Send UDP packet
void send_packet(char* data, int data_len) {
    char buffer[1500];
    struct iphdr* iph = (struct iphdr*)buffer;
    struct udphdr* udph = (struct udphdr*)(buffer + sizeof(struct iphdr));
    char* payload = buffer + sizeof(struct iphdr) + sizeof(struct udphdr);
    
    
    memcpy(payload, data, data_len);
    
    // UDP header
    udph->source = htons(54321); 
    udph->dest = server_addr.sin_port; 
    udph->len = htons(sizeof(struct udphdr) + data_len);
    udph->check = 0;
    
    // IP header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
    iph->id = htons(54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    if (server_addr.sin_addr.s_addr == inet_addr("127.0.0.1")) {
        iph->saddr = inet_addr("127.0.0.1");
        printf("DEBUG: Using 127.0.0.1 as source IP (local server)\n");
    } else {
        iph->saddr = get_interface_ip().s_addr;
        printf("DEBUG: Using interface IP %s as source IP\n", 
               inet_ntoa(*(struct in_addr*)&iph->saddr));
    }
    iph->daddr = server_addr.sin_addr.s_addr;
    iph->check = checksum(iph, sizeof(struct iphdr));
    
    
    
    // Send packet
    if (sendto(sockfd, buffer, sizeof(struct iphdr) + sizeof(struct udphdr) + data_len,
               0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) perror("sendto");
    
}

// Signal handler
void handle_signal(int sig) {
    running = 0;
    send_packet("CLOSE", 5);
    close(sockfd);
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    
    
    int one = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("DEBUG: setsockopt failed");
        exit(1);
    }
    
    
    
    // Server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid server IP address\n");
        exit(1);
    }
    
    
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    char input[1024];
    char buffer[1500];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    
    printf("Client started, connecting to %s:%u\n", 
           argv[1], atoi(argv[2]));
    struct in_addr server_ip = get_interface_ip();
    printf("DEBUG: Server IP address: %s\n", inet_ntoa(server_ip));
    
    
    while (running) {
        
        printf("Enter message (or 'quit' to exit): ");
        if (!fgets(input, sizeof(input), stdin)) break;
        
        input[strcspn(input, "\n")] = 0; 
        if (strcmp(input, "quit") == 0) {
            handle_signal(SIGINT);
            break;
        }
        
        // Send message
        send_packet(input, strlen(input));
        
        // Receive response
        while (1) {
            int len = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                              (struct sockaddr*)&from_addr, &from_len);
            if (len < 0) {
                if (errno == EINTR) continue;
                perror("recvfrom");
                break;
            }
            
            
            struct iphdr* iph = (struct iphdr*)buffer;
            struct udphdr* udph = (struct udphdr*)(buffer + (iph->ihl * 4));
            char* data = buffer + (iph->ihl * 4) + sizeof(struct udphdr);
            int data_len = len - (iph->ihl * 4) - sizeof(struct udphdr);
            
            // Filter packets
            if (iph->protocol != IPPROTO_UDP ||
                iph->saddr != server_addr.sin_addr.s_addr ||
                ntohs(udph->source) != ntohs(server_addr.sin_port)||
                ntohs(udph->dest) != 54321) continue;

            if (data_len > 0) {
                data[data_len] = '\0';
                printf("Received: %s\n", data);
                break;
            } 
        }
    }
    
    
    handle_signal(SIGINT);
    return 0;
}