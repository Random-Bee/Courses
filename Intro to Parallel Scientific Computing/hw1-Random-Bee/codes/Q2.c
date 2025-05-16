#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_FILENAME 200

// Function to free a matrix
void free_matrix(double** matrix, int n) {
    int i;
    for (i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to read an n x n matrix from a file
double** read_matrix(const char* filename, int n) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening matrix file");
        return NULL;
    }

    double** matrix = (double**)malloc(n * sizeof(double*));
    if (!matrix) {
        printf("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    int i, j;

    for (i = 0; i < n; i++) {
        matrix[i] = (double*)malloc(n * sizeof(double));
        if (!matrix[i]) {
            printf("Memory allocation failed");
            for (j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            fclose(file);
            return NULL;
        }
        for (j = 0; j < n; j++) {
            if (fscanf(file, "%lf,", &matrix[i][j]) != 1) {
                printf("Error reading matrix file\n");
                fclose(file);
                free_matrix(matrix, n);
                return NULL;
            }
        }
    }

    fclose(file);
    return matrix;
}

// Function to read a vector from a file
double* read_vector(const char* filename, int n) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    double* vector = (double*)malloc(n * sizeof(double));
    if (!vector) {
        printf("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    int i;

    for (i = 0; i < n; i++) {
        if (fscanf(file, "%lf,", &vector[i]) != 1) {
            printf("Error reading vector file\n");
            free(vector);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return vector;
}

// Function to check if a vector is an eigenvector of a matrix
bool is_eigenvector(double** matrix, double* vector, int n, double* eigenvalue) {
    double* result = (double*)malloc(n * sizeof(double));
    if (!result) {
        printf("Memory allocation failed");
        return false;
    }

    int i, j;

    // Matrix-vector multiplication
    for (i = 0; i < n; i++) {
        result[i] = 0;
        for (j = 0; j < n; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    double eps = 1e-6;

    // Find the first non-zero element of the vector
    for(i = 0; i < n; i++) {
        if(vector[i] > eps || vector[i] < -eps) {
            *eigenvalue = result[i] / vector[i];
            break;
        }
    }

    // Check if result is a scalar multiple of vector
    for(i = 0; i < n; i++) {
        // Check if vector[i] is zero and result[i] is non-zero
        bool fl1 = (vector[i] < eps && vector[i] > -eps) && (result[i] > eps || result[i] < -eps);
        // Check if vector[i] is non-zero and the ratio is not equal to the eigenvalue
        bool fl2 = (vector[i] > eps || vector[i] < -eps) && ((result[i] / vector[i]) - *eigenvalue > eps || (result[i] / vector[i]) - *eigenvalue < -eps);
        if(fl1 || fl2) {
            free(result);
            return false;
        } 
    }

    free(result);
    return true;
}

// Function to append the eigenvalue to the vector file
void write_eigenvalue(const char* filename, double eigenvalue) {
    FILE* file = fopen(filename, "a");
    if (!file) {
        printf("Error opening vector file for appending");
        return;
    }

    fprintf(file, "\nEigenvalue: %f\n", eigenvalue);
    fclose(file);
}

int main() {
    // Read n from input.in
    FILE* input_file = fopen("input.in", "r");
    if (!input_file) {
        printf("Error opening input.in");
        return 0;
    }

    int n;
    if (fscanf(input_file, "%d", &n) != 1) {
        printf("Error reading matrix size from input.in");
        fclose(input_file);
        return 0;
    }
    fclose(input_file);

    // Read matrix
    char matrix_filename[MAX_FILENAME];
    snprintf(matrix_filename, sizeof(matrix_filename), "../inputfiles/mat_%06d.in", n);
    double** matrix = read_matrix(matrix_filename, n);
    if (!matrix) {
        return 0;
    }

    int vecnum;

    // Process each vector
    for (vecnum = 1; ; vecnum++) {
        char vector_rel_filename[MAX_FILENAME];
        snprintf(vector_rel_filename, sizeof(vector_rel_filename), "../inputfiles/vec_%06d_%06d.in", n, vecnum);

        double* vector = read_vector(vector_rel_filename, n);
        // If vector is not read, free the matrix and return
        if (!vector) {
            free_matrix(matrix, n);
            return 0;
        }

        char vector_filename[MAX_FILENAME];
        snprintf(vector_filename, sizeof(vector_filename), "vec_%06d_%06d.out", n, vecnum);
        double eigenvalue;
        if (is_eigenvector(matrix, vector, n, &eigenvalue)) {
            printf("%s : Yes : %f\n", vector_filename, eigenvalue);
            write_eigenvalue(vector_rel_filename, eigenvalue);
        } else {
            printf("%s : Not an eigenvector\n", vector_filename);
        }

        free(vector);
    }

    free_matrix(matrix, n);
    return 0;
}
