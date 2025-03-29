
#include <stdio.h>
int subtract(double a, double b, double* result) {
    if (!result) return -1;
    *result = a - b;
    return 0;
}
