#include "header.h"

double calculate_square_area(double side) {
    return side * side;
}

void process_squares(double *sides, int start, int end, double *results) { 
    for (int i = start; i < end; i++) {
        results[i] = calculate_square_area(sides[i]);
    }
}