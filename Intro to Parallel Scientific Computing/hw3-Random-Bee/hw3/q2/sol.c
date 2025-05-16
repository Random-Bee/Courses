#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

const int N = 80; // Number of rows
const int M = 80; // Number of columns
const int vecnum = 1;

int rank, size;

double max(double a, double b) {
    return a > b ? a : b;
}

double min(double a, double b) {
    return a < b ? a : b;
}

double __abs(double a) {
    return a > 0 ? a : -a;
}

void read_matrix_rows(double local_matrix[][M], int local_rows) {
    int i, j;
    char filename[100];
    sprintf(filename, "inputfiles/mat_%06d.in", N);

    FILE *file = fopen(filename, "r");

    if(file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    // Move to the starting row for this process
    for(i = 0; i < rank * local_rows; i++) {
        for(j = 0; j < M; j++) {
            double temp;
            fscanf(file, "%lf,", &temp);
        }
    }
    
    // Read only the assigned rows
    for(i = 0; i < local_rows; i++) {
        for(j = 0; j < M; j++) {
            fscanf(file, "%lf,", &local_matrix[i][j]);
        }
    }
    fclose(file);
}


// Max and min values among all eigenvalues. If an eigenvalue exits, max and min values should be equal.
double max_eig, min_eig;

// Function to calculate the eigenvalues of a matrix
void calculate_eigenvalue(double local_matrix[][M], int local_rows) {
    int i, j;
    char vector_file[100];
    sprintf(vector_file, "inputfiles/vec_%06d_%06d.in", M, vecnum);
    double vector[M];
    FILE *file = fopen(vector_file, "r");

    if(file == NULL) {
        printf("Error opening file %s\n", vector_file);
        return;
    }

    for(i=0; i<M; i++) {
        fscanf(file, "%lf,", &vector[i]);
    }
    fclose(file);

    // taking -1e18 and 1e18 as -INF and INF
    // At the end, if a vector is an eigenvector, max and min will be equal
    // It may happen that the process ends up getting all the rows from which no information can be extracted
    // In such case, max and min will be -1e18 and 1e18 respectively, not affecting the result
    max_eig = -1e18;
    min_eig = 1e18;

    // Iterate over all the rows assigned to this process
    for(i=0; i<local_rows; i++) {
        double sum = 0;
        for(j=0; j<M; j++) {
            sum += local_matrix[i][j] * vector[j];
        }

        // Index of the vector corresponding to the current row
        j = i + rank * local_rows;

        // sum is not zero and vector[j] is zero. This means that the vector is not an eigenvector
        if(__abs(sum) > 1e-9 && __abs(vector[j]) < 1e-9) {
            max_eig = 1e18;
            min_eig = -1e18;
            continue;
        }
        // since sum is zero, we do not get any information about the eigenvalues
        if(__abs(sum) < 1e-9) {
            printf("Here\n");
            continue;
        }
        // sum is not zero and vector[j] is not zero. This means that we find a possible eigenvalue
        if(__abs(vector[j]) > 1e-9) {
            double eig = sum / vector[j];
            max_eig = max(max_eig, eig);
            min_eig = min(min_eig, eig);
        }
    }

    // Reduce the max and min values to the root process
    double max_eig_final = -1e18, min_eig_final = 1e18;
    MPI_Reduce(&max_eig, &max_eig_final, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&min_eig, &min_eig_final, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        if(__abs(max_eig_final - min_eig_final) < 1e-9) {
            printf("Yes, eigenvalue = %lf\n", i, max_eig);
        }
        else {
            printf("No\n");
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_rows = N / size;
    double local_matrix[local_rows][M];
    
    read_matrix_rows(local_matrix, local_rows);

    calculate_eigenvalue(local_matrix, local_rows);
    
    MPI_Finalize();
    return 0;
}
