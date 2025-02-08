// Aayush Kumar
// CO21BTECH11002
// OS1 Assignmnet 2
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
struct values {
    int begin; // the number from where the thread starts checking for perfect numbers
    int end; // the number till where the thread checks for perfect numbers (inclusive)
    int count; // count of perfect numbers that the thread finds
    int numbers[100]; // store the perfect numbers found by the thread
    int in; // index of the thread
};
// funtion to find the perfect numbers
void* getPerfectNumbers(void* val1) {
    struct values *val = (struct values*) val1;
    int i,j,sum;
    FILE* f;
    char buffer[100];
    sprintf(buffer,"OutFile%d.txt", val->in);
    // creates the log file
    f = fopen(buffer,"w");
    // initialize count to be 0
    val->count = 0;
    // for each number lying within the thread's domain, check if it is perfect or not
    for(i=val->begin; i<=val->end; i++) {
        sum=1; // initialize sum to be 1
        // start checking from to till sqrt(i)
        for(j=2; j*j<=i; j++) {
            if(i%j==0) {
                if(j*j != i) sum += j + i/j;
                else sum += j;
            }
        }
        // if perfect number, store it in the numbers array and increment the count
        if(i!=1 && sum==i) {
            sprintf(buffer, "%d is a perfect number.\n", i);
            fputs(buffer,f);
            val->numbers[val->count] = i;
            val->count++;
        }
        else {
            sprintf(buffer, "%d is not a perfect number.\n", i);
            fputs(buffer,f);
        }
    }
    fclose(f);
}
int min(int a, int b) {
    return a<b? a: b;
}
int main(int argc, char* argv[]) {
    int n,k,ct;
    FILE* f1;
    // open the input file and read the value of n and k
    f1 = fopen(argv[1], "r");
    fscanf(f1, "%d %d", &n, &k);
    fclose(f1);
    // ct holds the number of integers each thread will get to check
    ct = n/k;
    if(n%k!=0) ct++;
    // initializes k threads and corresponding values
    pthread_t *threads;
    threads = (pthread_t*)malloc(sizeof(pthread_t)*k);
    struct values *threadValues;
    threadValues = (struct values*)malloc(sizeof(struct values)*k);
    // initializes the values associated with each thread
    for(int i=0; i<k; i++) {
        threadValues[i].begin = i*ct+1;
        threadValues[i].end = min((i+1)*ct,n);
        threadValues[i].in = i;
    }
    // creates all the threads and run them
    for(int i=0; i<k; i++) {
        pthread_create(&threads[i], NULL, getPerfectNumbers, &threadValues[i]);
    }
    // waits for the threads to finish and join them
    for(int i=0; i<k; i++) {
        pthread_join(threads[i], NULL);
    }
    // creates the OutMain file
    char buffer[100];
    f1 = fopen("OutMain.txt", "w");
    for(int i=0; i<k; i++) {
        // if the thread finds atleast one perfect number then insert them in the OutMain file
        if(threadValues[i].count != 0) {
            sprintf(buffer, "Thread%d:", i+1);
            fputs(buffer, f1);
            for(int j=0; j<threadValues[i].count; j++) {
                sprintf(buffer, " %d", threadValues[i].numbers[j]);
                fputs(buffer, f1);
            }
            fputs("\n", f1);
        }
    }
    fclose(f1);
    // frees the allocated memory
    free(threads);
    free(threadValues);
    return 0;
}