// Aayush Kumar
// CO21BTECH11002
// OS2 Assignmnet 1
#include <iostream>
#include <pthread.h>
#include <chrono>
#include<random>
using namespace std;
using namespace std::chrono;

struct values {
    int n; // the number of points the thread needs to check
    double *pointsX; // array storing the x-coords, points inside the circle will be stored at the begining and points outside the circle stored at the end
    double *pointsY; // array storing the y-coords, points inside the circle will be stored at the begining and points outside the circle stored at the end
    int count; // count of points that lie inside the circle
};

// funtion to check wether points lie inside circle or not
// I have taken the circle to be x^2 + y^2 = 1 (i.e., center at origin)
// The square will have coordinates as (1,1), (1,-1), (-1,1), (-1,-1)
void* checkPoints(void* val1) {
    struct values *val = (struct values*) val1;
    int i,st=0,en=val->n-1;
    double x,y;

    // Create two arrays for stroing the points
    val->pointsX = new double[val->n];
    val->pointsY = new double[val->n];

    // initialize count of points inside circle to be 0
    val->count = 0;

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    for(i=0; i<val->n; i++) {
        // get two points between -1 and 1 (so that the points remain inside the square).
        x = dis(gen);
        y = dis(gen);

        // point lies inside the the circle
        if(x*x + y*y <= 1) {
            val->count++;
            val->pointsX[st]=x;
            val->pointsY[st]=y;
            st++;
        }

        // point lies outside the circle but inside the square
        else {
            val->pointsX[en]=x;
            val->pointsY[en]=y;
            en--;
        }
    }
    return 0;
}
int main(int argc, char* argv[]) {
    int n,k,ct;
    FILE* f1;

    // open the input file and read the value of n and k
    f1 = fopen(argv[1], "r");
    fscanf(f1, "%d %d", &n, &k);
    fclose(f1);

    // ct holds the number of points each thread will get to check
    ct = n/k;
    if(n%k!=0) ct++;

    // initializes k threads and corresponding values
    pthread_t *threads;
    threads = (pthread_t*)malloc(sizeof(pthread_t)*k);
    struct values *threadValues;
    threadValues = (struct values*)malloc(sizeof(struct values)*k);

    // initializes the values associated with each thread
    for(int i=0; i<k; i++) {
        threadValues[i].n = ct;
    }

    auto start = high_resolution_clock::now();
    // creates all the threads and run them
    for(int i=0; i<k; i++) {
        pthread_create(&threads[i], NULL, checkPoints, &threadValues[i]);
    }
    // waits for the threads to finish and join them
    for(int i=0; i<k; i++) {
        pthread_join(threads[i], NULL);
    }
    int inCircle=0; // total no. of point inside circle found by all the threads
    for(int i=0; i<k; i++) {
        inCircle += threadValues[i].count;
    }
    double pi = (4.0*inCircle)/n;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    // creates the Output file and prints required data
    char buffer[100];
    f1 = fopen("Output.txt", "w");
    sprintf(buffer, "Time: %ld \xC2\xB5s\n\nValue computed: %lf\n\nLog:\n\n", duration.count(), pi);
    fputs(buffer, f1);

    for(int i=0; i<k; i++) {

        sprintf(buffer, "Thread%d: %d, %d\n", i+1, ct, threadValues[i].count);
        fputs(buffer, f1);

        sprintf(buffer, "Points inside the square and outside the circle: ");
        fputs(buffer, f1);

        for(int j=threadValues[i].count; j<ct; j++) {
            sprintf(buffer, "(%lf, %lf), ", threadValues[i].pointsX[j], threadValues[i].pointsY[j]);
            fputs(buffer, f1);
        }
        sprintf(buffer, "(%lf, %lf)\n", threadValues[i].pointsX[ct-1], threadValues[i].pointsY[ct-1]);
        fputs(buffer, f1);

        sprintf(buffer, "Points inside the circle: ");
        fputs(buffer, f1);

        for(int j=0; j<threadValues[i].count; j++) {
            sprintf(buffer, "(%lf, %lf), ", threadValues[i].pointsX[j], threadValues[i].pointsY[j]);
            fputs(buffer, f1);
        }
        sprintf(buffer, "(%lf, %lf)\n\n", threadValues[i].pointsX[threadValues[i].count-1], threadValues[i].pointsY[threadValues[i].count-1]);
        fputs(buffer, f1);
    }
    fclose(f1);

    // frees the allocated memory
    free(threads);
    for(int i=0; i<k; i++) {
        free(threadValues[i].pointsX);
        free(threadValues[i].pointsY);
    }
    free(threadValues);
    return 0;
}