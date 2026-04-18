
int compare_int_arrays(int *a, int *b, int len) {
    int result = 1;
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            result = 0;
        }
    }
    return result;
}

// precision: delta = 1e-8
int compare_double_arrays(double *a, double *b, int len) {
    int result = 1;
    for (int i = 0; i < len; i++) {
        if ((a[i] < b[i] - 1e-8) || (a[i] > b[i] + 1e-8)) {
            result = 0;
        }
    }
    return result;
}


