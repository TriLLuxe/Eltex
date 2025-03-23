#include "header.h"
#include <stdio.h>
#include <stdarg.h>
int add(int count,double *result, ...) {
    if (count < 1 || !result) return -1; // Ошибка: неверные аргументы
    va_list args;
    va_start (args, count);
    *result = va_arg(args, double);
    for (int i = 1; i < count; i++) {
        *result += va_arg(args, double);
    }
    va_end(args);
    return 0;
}

int subtract(int count,double *result, ...) {
    if (count < 1 || !result) return -1;
    va_list args;
    va_start (args, count);
    *result = va_arg(args, double);
    for (int i = 1; i < count; i++) {
        *result -= va_arg(args, double);
    }
    va_end(args);
    return 0;
}

int multiply(int count,double *result, ...) {
    if (count < 1 || !result) return -1;
    va_list args;
    va_start (args, count);
    *result = va_arg(args, double);
    for (int i = 1; i < count; i++) {
        *result *= va_arg(args, double);
    }
    va_end(args);
    return 0;
}   

int divide(int count,double *result, ...) {

    if (count < 1 || !result) return -1;
    va_list args;
    va_start (args, count);
    *result = va_arg(args, double);
    for (int i = 1; i < count; i++) {
        double next = va_arg(args, double);
        if (next == 0) return -2;// Ошибка: деление на ноль
        *result /= next;
    }
    va_end(args);
    return 0;
}