#include "header.h"
#include <stdio.h>
#include <stdarg.h>
double add(int count, ...) {
    if (count < 1){
        printf("Ошибка: неверное количество аргументов\n");
        return 0;
    }
    va_list args;
    va_start (args, count);
    double result = va_arg(args, double);
    for (int i = 1; i < count; i++) {
        result += va_arg(args, double);
    }
    va_end(args);
    return result;
}

double subtract(int count, ...) {
    if (count < 1){
        printf("Ошибка: неверное количество аргументов\n");
        return 0;
    }
    va_list args;
    va_start (args, count);
    double result = va_arg(args, double);
    for (int i = 1; i < count; i++) {
        result -= va_arg(args, double);
    }
    va_end(args);
    return result;
}

double multiply(int count, ...) {
    if (count < 1){
        printf("Ошибка: неверное количество аргументов\n");
        return 0;
    }
    va_list args;
    va_start (args, count);
    double result = va_arg(args, double);
    for (int i = 1; i < count; i++) {
        result *= va_arg(args, double);
    }
    va_end(args);
    return result;
}   

double divide(int count, ...) {

    if (count < 1){
        printf("Ошибка: неверное количество аргументов\n");
        return 0;
    }
    va_list args;
    va_start (args, count);
    double result = va_arg(args, double);
    for (int i = 1; i < count; i++) {
        double next = va_arg(args, double);
        if (next == 0) {
            printf("Ошибка: деление на ноль\n");
            return 0;
        }
        result /= next;
    }
    va_end(args);
    return result;
}