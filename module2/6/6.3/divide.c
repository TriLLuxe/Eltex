
#include <stdio.h>
int divide(double a, double b, double* result) {
    if (!result) return -1;
    if (b == 0) return -2; // Ошибка: деление на ноль
    *result = a / b;
    return 0;
}
