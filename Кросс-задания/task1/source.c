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

// Checksum calculation
unsigned short checksum(void* data, int len) {
    unsigned long sum = 0;
    unsigned short* buf = data;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len) sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}
struct in_addr get_interface_ip() {
    struct ifaddrs *ifaddr, *ifa;
    struct in_addr ip = {0};

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(1);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            if (strcmp(ifa->ifa_name, "lo") != 0) { //loopback exclude
                ip = sa->sin_addr;
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
    return ip;
}