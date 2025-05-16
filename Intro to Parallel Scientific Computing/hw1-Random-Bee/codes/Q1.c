#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_to_file(int n, double **A, int format_flag) {
    char filename[32];
    snprintf(filename, sizeof(filename), "array_%06d_%s.out", n, format_flag == 0 ? "asc" : "bin");

    FILE *output_file = fopen(filename, format_flag == 0 ? "w" : "wb");
    if (output_file == NULL) {
        printf("Error creating output file\n");
        return;
    }

    int i, j;

    if (format_flag == 0) {
        // Write in ASCII format
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                // 15 decimal places
                fprintf(output_file, "%.15f%s", A[i][j], j == n - 1 ? "\n" : " ");
            }
        }
    } else {
        // Write in binary format
        for (i = 0; i < n; i++) {
            if (fwrite(A[i], sizeof(double), n, output_file) != n) {
                printf("Error writing to binary file\n");
                fclose(output_file);
                return;
            }
        }
    }

    fclose(output_file);
}

int main() {
    FILE *input_file = fopen("input_1.in", "r");
    if (input_file == NULL) {
        printf("Error opening file input.in");
        return 0;
    }

    int n, i, j;
    if (fscanf(input_file, "%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid matrix size in input.in\n");
        fclose(input_file);
        return 0;
    }
    fclose(input_file);

    // Dynamically allocate 2D array A
    double **A = (double **)malloc(n * sizeof(double *));
    if (A == NULL) {
        printf("Memory allocation failed");
        return 0;
    }

    for (i = 0; i < n; i++) {
        A[i] = (double *)malloc(n * sizeof(double));
        if (A[i] == NULL) {
            printf("Memory allocation failed");
            for (j = 0; j < i; j++) {
                free(A[j]);
            }
            free(A);
            return 0;
        }
    }

    // Fill the matrix A with A[i][j] = i + j
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            A[i][j] = i + j;
        }
    }

    // Call print_to_file for both ASCII and binary formats
    print_to_file(n, A, 0); // ASCII format
    print_to_file(n, A, 1); // Binary format

    // Free allocated memory
    for (i = 0; i < n; i++) {
        free(A[i]);
    }
    free(A);

    return 0;
}
