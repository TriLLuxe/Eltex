#include "header.h"

int add(double a, double b, double* result) {
    if (!result) return -1; 
    *result = a + b;
    return 0; 
}

int subtract(double a, double b, double* result) {
    if (!result) return -1;
    *result = a - b;
    return 0;
}

int multiply(double a, double b, double* result) {
    if (!result) return -1;
    *result = a * b;
    return 0;
}

int divide(double a, double b, double* result) {
    if (!result) return -1;
    if (b == 0) return -2; // Ошибка: деление на ноль
    
    *result = a / b;
    return 0;
}