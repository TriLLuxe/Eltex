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
#include <errno.h>
#include <ifaddrs.h>
#include "header.h"


// Structure for client identification
typedef struct {
    struct in_addr ip;
    uint16_t port;
    uint32_t msg_count;
} Client;

// Hash table node
typedef struct Node {
    Client client;
    struct Node* next;
} Node;

// Hash table
#define TABLE_SIZE 256
Node* hash_table[TABLE_SIZE];
int sockfd;
volatile sig_atomic_t running = 1;



// Hash function
unsigned int hash(struct in_addr ip, uint16_t port) {
    return ((unsigned int)ip.s_addr ^ port) % TABLE_SIZE;
}

// Find or create client
Client* find_client(struct in_addr ip, uint16_t port) {
    unsigned int index = hash(ip, port);
    Node* current = hash_table[index];
    
    while (current) {
        if (current->client.ip.s_addr == ip.s_addr && current->client.port == port) return &current->client;
        current = current->next;
    }
    Node* new_node = malloc(sizeof(Node));
    new_node->client.ip = ip;
    new_node->client.port = port;
    new_node->client.msg_count = 0;
    new_node->next = hash_table[index];
    hash_table[index] = new_node;
    return &new_node->client;
}

// Remove client
void remove_client(struct in_addr ip, uint16_t port) {
    unsigned int index = hash(ip, port);
    Node* current = hash_table[index];
    Node* prev = NULL;
    
    while (current) {
        if (current->client.ip.s_addr == ip.s_addr && current->client.port == port) {
            if (prev) prev->next = current->next;
            else hash_table[index] = current->next;
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Signal handler
void handle_signal(int sig) {
    running = 0;
}

// Send response
void send_response(struct in_addr src_ip, uint16_t src_port, char* data, int data_len, uint32_t msg_count) {
    char buffer[1500];
    struct iphdr* iph = (struct iphdr*)buffer;
    struct udphdr* udph = (struct udphdr*)(buffer + sizeof(struct iphdr));
    char* _data = buffer + sizeof(struct iphdr) + sizeof(struct udphdr);
    
    // Prepare _data
    snprintf(_data, 1500 - sizeof(struct iphdr) - sizeof(struct udphdr), 
             "%s %u", data, msg_count);
    int _data_len = strlen(_data);
    
    // UDP header
    udph->source = htons(51000); // Server port
    udph->dest = htons(src_port);
    udph->len = htons(sizeof(struct udphdr) + _data_len);
    udph->check = 0;
    
    // IP header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + _data_len);
    iph->id = htons(54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    if (src_ip.s_addr == inet_addr("127.0.0.1")) {
        iph->saddr = inet_addr("127.0.0.1");
    } else {
        iph->saddr = get_interface_ip().s_addr;
        
    }
    iph->daddr = src_ip.s_addr;
    iph->check = checksum(iph, sizeof(struct iphdr));

    // Send packet
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr = src_ip;
    
    if (sendto(sockfd, buffer, sizeof(struct iphdr) + sizeof(struct udphdr) + _data_len,
               0, (struct sockaddr*)&dest, sizeof(dest)) < 0) {
        perror("sendto");
    }
}

int main(int argc, char* argv[]) {
    
    memset(hash_table, 0, sizeof(hash_table));
    
    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("socket creation");
        exit(1);
    }
    
    
    int one = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        exit(1);
    }
    
    
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    char buffer[1500];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    printf("Server started, listening for UDP packets on port 51000\n");
    struct in_addr server_ip = get_interface_ip();
    printf("DEBUG: Server IP address: %s\n", inet_ntoa(server_ip));

    
    while (running) {
        // Receive packet
        int len = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                          (struct sockaddr*)&client_addr, &addr_len);
        if (len < 0) {
            if (errno == EINTR) continue;
            perror("recvfrom");
            continue;
        }
        
        // Parse packet
        struct iphdr* iph = (struct iphdr*)buffer;
        struct udphdr* udph = (struct udphdr*)(buffer + (iph->ihl * 4));
        char* data = buffer + (iph->ihl * 4) + sizeof(struct udphdr);
        int data_len = len - (iph->ihl * 4) - sizeof(struct udphdr);
        
        if (data_len <= 0 || iph->protocol != IPPROTO_UDP) continue;
        
        
        // Get client info
        struct in_addr src_ip = {iph->saddr};
        uint16_t src_port = ntohs(udph->source);
        char data_str[1500];
        strncpy(data_str, data, data_len);
        data_str[data_len] = '\0';
    
        
        // Check for close message
        if (strncmp(data, "CLOSE", 5) == 0) {
            remove_client(src_ip, src_port);
            continue;
        }
        
        // Increment message count and send response
        Client* client = find_client(src_ip, src_port);
        client->msg_count++;
        send_response(src_ip, src_port, data_str, data_len, client->msg_count);
    }
    
   
    printf("exiting\n");
    close(sockfd);
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = hash_table[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    return 0;
}