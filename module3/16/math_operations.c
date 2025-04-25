#include <stdio.h>
#include "math_operations.h"

double add(double a, double b, int *status) {
    *status = 0;
    return a + b;
}

double sub(double a, double b, int *status) {
    *status = 0;
    return a - b;
}

double mul(double a, double b, int *status) {
    *status = 0;
    return a * b;
}

double _div(double a, double b, int *status) {
    if (b == 0) {
        fprintf(stderr, "ERROR: Division by zero\n");
        *status = 1;
        return 0;
    }
    *status = 0;
    return a / b;
}