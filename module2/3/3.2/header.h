#ifdef HEADER_H
#define HEADER_H
#include <stdint.h>
//Ф-ия для преобразования IP-адреса из строки в int32
uint32_t ip_to_int(const char* ip);
//Ф-ия для генерации IP-адреса
uint32_t generate_random_ip();


#   endif