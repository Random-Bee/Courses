// Aayush Kumar
// CO21BTECH11002
// OS2 Assignmnet 2
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

int **sudoku;

struct values {
    int size; // Size of sides of sudoku
    int num_row; // Number of rows the thread has to check
    int num_col; // Number of columns the thread has to check
    int num_grid; // Number of grids the thread has to check
    int *indices_row; // Indices of the rows the thread has to check
    int *indices_col; // Indices of the columns the thread has to check
    int *indices_grid; // Indices of the grids the thread has to check
    int *valid_row; // Stores whether the row with given index is valid or not
    int *valid_col; // Stores whether the column with given index is valid or not
    int *valid_grid; // Stores whether the grid with given index is valid or not
};

// Function to check the validity of sets of rows, columns, and grids
void* checkValid(void* val1) {
    struct values *val = (struct values*) val1;

    int *freq; // stores frequency of each element in the row/column/grid
    int n = val->size, i, j, k, flag, in, m=sqrt(n);
    freq = (int*)malloc((n+1)*sizeof(int));
    
    // Check for given rows
    for(j=0; j<val->num_row; j++) {
        
        in = val->indices_row[j]; // row index to check
        
        flag=1; // assume the row is valid, so, flag is 1.
        
        for(i=0; i<=n; i++) freq[i]=0; // set frequency of each element to be 0
        
        for(i=0; i<n; i++) {
            
            if(sudoku[in][i]>=1 && sudoku[in][i]<=n) freq[sudoku[in][i]]++;
            else flag = 0; // invalid number

            if(freq[sudoku[in][i]]>1) { // a number appeared twice
                flag =  0;
            }
        }
        if(!flag) val->valid_row[j] = 0; // If flag becomes 0, row is invalid
        else val->valid_row[j] = 1; // If flag remains 1, row is valid
    }

    // Check for given columns
    for(j=0; j<val->num_col; j++) {

        in = val->indices_col[j]; // column index to check
        
        flag=1; // assume the column is valid, so, flag is 1.

        for(i=0; i<=n; i++) freq[i]=0; // set frequency of each element to be 0
        
        for(i=0; i<n; i++) {
            
            if(sudoku[i][in]>=1 && sudoku[i][in]<=n) freq[sudoku[i][in]]++;
            else flag = 0; // invalid number
            
            if(freq[sudoku[i][in]]>1) { // a number appeared twice
                flag =  0;
            }
        }
        if(!flag) val->valid_col[j] = 0; // If flag becomes 0, column is invalid
        else val->valid_col[j] = 1; // If flag remains 1, column is valid
    }

    // Check for grids
    for(j=0; j<val->num_grid; j++) {

        in = val->indices_grid[j]; // column index to check
        int x = (in/m)*m, y = (in%m)*m; // grid with index in will start from coordinates (in/m)*m, (in%m)*m in the sudoku
        
        flag=1; // assume the grid is valid, so, flag is 1.
        
        for(i=0; i<=n; i++) freq[i]=0; // set frequency of each element to be 0
        
        for(i=0; i*i<n; i++) {
            for(k=0; k*k<n; k++) {

                if(sudoku[x+i][y+k]>=1 && sudoku[x+i][y+k]<=n)freq[sudoku[x+i][y+k]]++;
                else { // invalid number
                    flag=0;
                }
                
                if(freq[sudoku[x+i][y+k]]>1) { // A number appeared twice
                    flag =  0;
                }
            }
        }
        if(!flag) val->valid_grid[j] = 0; // If flag becomes 0, grid is invalid
        else val->valid_grid[j] = 1; // If flag remains 1, grid is valid
    }

    free(freq);
}

int main(int argc, char* argv[]) {
    int n,k,i,j,p,q,m;
    FILE* f1;

    // Open the input file and read the varibles.
    f1 = fopen(argv[1], "r");
    fscanf(f1, "%d %d", &k, &n);

    sudoku = (int**)malloc(n*sizeof(int*));
    for(i=0; i<n; i++) {
        sudoku[i] = (int*)malloc(n*sizeof(int));
    }

    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            fscanf(f1, "%d",&sudoku[i][j]);
        }
    }
    fclose(f1);

    // initializes k threads and corresponding values
    pthread_t *threads;
    threads = (pthread_t*)malloc(sizeof(pthread_t)*k);
    struct values *threadValues;
    threadValues = (struct values*)malloc(sizeof(struct values)*k);

    // initializes the values associated with each thread
    int ct = 0, in, l=0;
    for(i=0; i<k; i++) {
        threadValues[i].size = n;
        // each thread currently holds no indices
        threadValues[i].num_row = 0;
        threadValues[i].num_col = 0;
        threadValues[i].num_grid = 0;
        // each thread will check at max n/k + 1 rows/columns/grids
        threadValues[i].indices_row = (int*)calloc(n/k+1,(n/k+1)*sizeof(int));
        threadValues[i].indices_col = (int*)calloc(n/k+1,(n/k+1)*sizeof(int));
        threadValues[i].indices_grid = (int*)calloc(n/k+1,(n/k+1)*sizeof(int));
        threadValues[i].valid_row = (int*)calloc(n/k+1,(n/k+1)*sizeof(int));
        threadValues[i].valid_col = (int*)calloc(n/k+1,(n/k+1)*sizeof(int));
        threadValues[i].valid_grid = (int*)calloc(n/k+1,(n/k+1)*sizeof(int));
    }

    /*
    Distribution scheme:
    Say we Have 4 threads and 9x9 sudoku then each thread gets data in the followin way:

    T1  T2  T3  T4

    R1  R2  R3  R4
    R5  R6  R7  R8
    R9  C1  C2  C3
    C4  C5  C6  C7
    C8  C9  G1  G2
    G3  G4  G5  G6
    G7  G8  G9

    R-row, C-column, G-grid
    
    */

    for(i=0; i<3*n; i++) {
        if(i<n) { // First fill rows
            in = i;
            threadValues[i%k].indices_row[threadValues[i%k].num_row] = in;
            threadValues[i%k].num_row++;
        }
        else if(i<2*n) { // Then fill columns
            in = i-n;
            threadValues[i%k].indices_col[threadValues[i%k].num_col] = in;
            threadValues[i%k].num_col++;
        }
        else { // Finally fill grids
            in = i-2*n;
            threadValues[i%k].indices_grid[threadValues[i%k].num_grid] = in;
            threadValues[i%k].num_grid++;
        }
    }

    struct timeval begin, end;
    gettimeofday(&begin, 0);

    int* threadIndices = (int*)calloc(k,k*sizeof(int));

    // Start the multithreading process.
    #pragma omp parallel for num_threads(k)
    // Each thread will execute exactly 1 iteration of for loop since we have k threads and k iterations.
    for(i=0; i<k; i++) {
        threadIndices[i] = omp_get_thread_num();
        checkValid(&threadValues[i]);
    }

    // Assume sudoku is valid, so, flag is 1.
    int flag = 1;
    for(i=0; i<k; i++) {
        for(j=0; j<threadValues[i].num_row; j++) {
            flag = threadValues[i].valid_row[j];
            if(flag==0) break; // An invalid row is found.
        }
        for(j=0; j<threadValues[i].num_col; j++) {
            flag = threadValues[i].valid_col[j];
            if(flag==0) break; // An invalid column is found.
        }
        for(j=0; j<threadValues[i].num_grid; j++) {
            flag = threadValues[i].valid_grid[j];
            if(flag==0) break; // An invalid grid is found.
        }
        if(flag==0) break;
    }

    gettimeofday(&end, 0);
    long elapsed = 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);

    char buffer[100];
    f1 = fopen("Output.txt", "w");

    for(i=0; i<k; i++) {
        // Output Row data
        for(j=0; j<threadValues[i].num_row; j++) {
            if(threadValues[i].valid_row[j]) {
                sprintf(buffer, "Thread %d checks row %d and is valid.\n", threadIndices[i], threadValues[i].indices_row[j]+1);
                fputs(buffer, f1);
            }
            else {
                sprintf(buffer, "Thread %d checks row %d and is invalid.\n", threadIndices[i], threadValues[i].indices_row[j]+1);
                fputs(buffer, f1);
            }
        }

        // Output Column data
        for(j=0; j<threadValues[i].num_col; j++) {
            if(threadValues[i].valid_col[j]) {
                sprintf(buffer, "Thread %d checks column %d and is valid.\n", threadIndices[i], threadValues[i].indices_col[j]+1);
                fputs(buffer, f1);
            }
            else {
                sprintf(buffer, "Thread %d checks column %d and is invalid.\n", threadIndices[i], threadValues[i].indices_col[j]+1);
                fputs(buffer, f1);
            }
        }

        // Output Grid data
        for(j=0; j<threadValues[i].num_grid; j++) {
            if(threadValues[i].valid_grid[j]) {
                sprintf(buffer, "Thread %d checks grid %d and is valid.\n", threadIndices[i], threadValues[i].indices_grid[j]+1);
                fputs(buffer, f1);
            }
            else {
                sprintf(buffer, "Thread %d checks grid %d and is invalid.\n", threadIndices[i], threadValues[i].indices_grid[j]+1);
                fputs(buffer, f1);
            }
        }

    }
    if(flag) { // If flag remains 1 then sudoku is valid.
        sprintf(buffer, "Sudoku is Valid.\n");
        fputs(buffer, f1);
    }
    else {
        sprintf(buffer, "Sudoku is invalid.\n");
        fputs(buffer, f1);
    }

    // Output time taken in microseconds
    sprintf(buffer, "Total time taken is %ld \xC2\xB5s\n", elapsed);

    fputs(buffer,f1);

    // Free all the allocated memory
    free(threads);
    for(i=0; i<k; i++) {
        free(threadValues[i].indices_row);
        free(threadValues[i].indices_col);
        free(threadValues[i].indices_grid);
        free(threadValues[i].valid_row);
        free(threadValues[i].valid_col);
        free(threadValues[i].valid_grid);
    }
    free(threadValues);
    free(threadIndices);
    for(i=0; i<n; i++) free(sudoku[i]);
    free(sudoku);

    return 0;
}