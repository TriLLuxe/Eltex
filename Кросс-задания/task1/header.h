#ifndef HEADER_H
#define HEADER_H    
#include <stdio.h>  
#include <stdlib.h>

unsigned short checksum(void* data, int len);
struct in_addr get_interface_ip();

#endif