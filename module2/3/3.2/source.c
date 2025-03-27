#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
uint32_t ip_to_int(const char *ip){
    uint8_t a,b,c,d;
    sscanf(ip,"%hhu.%hhu.%hhu.%hhu",&a,&b,&c,&d);
    return (a<<24)| (b<<16) | (c<<8)|d;
}
uint32_t generate_random_ip(){
    uint32_t ip = 0;
    // Генерируем каждый октет (0-255) и собираем 32-битный IP-адрес
    for (int i = 0; i < 4; i++) {

        ip |= (uint32_t)(rand() % 256) << (i * 8);
    }
    return ip;
}