#include <stdlib.h>

double *alloc_fill_double_array(int len, double default_val) {
    double *array = malloc(len * sizeof(double));
    for (int i = 0; i < len; i++) {
        array[i] = (double)default_val;
    }
    return array;
}
