#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double *alloc_fill_double_array(const int len, const double default_val) {
    double *array = malloc(len * sizeof(double));
    for (int i = 0; i < len; i++) {
        array[i] = (double)default_val;
    }
    return array;
}

int sum_int_array(const int *array, const int len) {
    int sum = 0;
    for (int i = 0; i < len; i++)
        sum += array[i];
    return sum;
}

DiffusionConfig read_config(const char *filename) {
    DiffusionConfig config = {0};

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char key[64];
    char val_str[64];

    while (fscanf(file, "%63s %63s", key, val_str) == 2) {

        if (strcmp(key, "type") == 0) {
            strcpy(config.type, val_str);
        } else if (strcmp(key, "run") == 0) {
            strcpy(config.run, val_str);
        } else if (strcmp(key, "dependency") == 0) {
            strcpy(config.dependency, val_str);
        } else if (strcmp(key, "boundary") == 0) {
            strcpy(config.boundary, val_str);
        } else if (strcmp(key, "init_density") == 0) {
            strcpy(config.init_density, val_str);
        } else if (strcmp(key, "delta_t") == 0) {
            config.delta_t = atof(val_str);
        } else if (strcmp(key, "start") == 0) {
            config.start = atof(val_str);
        } else if (strcmp(key, "lower_bound") == 0) {
            config.lower_bound = atof(val_str);
        } else if (strcmp(key, "upper_bound") == 0) {
            config.upper_bound = atof(val_str);
        } else if (strcmp(key, "p_0") == 0) {
            config.p_0 = atof(val_str);
        } else if (strcmp(key, "n_t") == 0) {
            config.n_t = atoi(val_str);
        } else if (strcmp(key, "n_realizations") == 0) {
            config.n_realizations = atoi(val_str);
        } else if (strcmp(key, "d") == 0) {
            config.d = atof(val_str);
        } else if (strcmp(key, "n_bins") == 0) {
            config.n_bins = atoi(val_str);
        } else if (strcmp(key, "alpha") == 0) {
            config.alpha = atoi(val_str);
        } else if (strcmp(key, "rs") == 0) {
            config.rs = atoi(val_str);
        } else if (strcmp(key, "c") == 0) {
            config.c = atof(val_str);
        } else if (strcmp(key, "q") == 0) {
            config.q = atoi(val_str);
        } else if (strcmp(key, "frame_timestep") == 0) {
            config.frame_timestep = atoi(val_str);

        } else if (strcmp(key, "comments:") == 0) { // from here starts the commensts section
            break;
        } else {
            printf("Warning: Unknown config key '%s' ignored.\n", key);
        }
    }
    fclose(file);

    return config;
}
