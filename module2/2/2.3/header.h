#ifndef HEADER_H
#define HEADER_H

// Тип указателя на функцию 
typedef int (*OperationFunc)(double, double, double*);


int add(double a, double b, double* result);
int subtract(double a, double b, double* result);
int multiply(double a, double b, double* result);
int divide(double a, double b, double* result);

#endif