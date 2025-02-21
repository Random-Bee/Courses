// Aayush Kumar
// CO21BTECH11002
// Assignment 1 - SAM1
#include <bits/stdc++.h>
#include <semaphore.h>
#include <sys/time.h>
using namespace std;

// SAM1

class valsSAM{
    public:
    // starting number for each thread
    int start;
    // array to store primes found by each thread
    vector<int> primes;
};

// no. of digits, no. of threads
int n,m;

// Prime finding function for each thread
void getPrimesSAM(valsSAM* val) {
    int start = val->start, end = pow(10, n), step = 2*m;
    // check for primes in range [start, end] with step size 2*m, 
    for(int i=start; i<=end; i+=step) {
        bool isPrime = true;
        if(i==1) {
            isPrime = false;
        }
        // check if current number is prime in sqrt(i) time
        for(int j=2; j*j<=i; j++) {
            if(i%j == 0) {
                isPrime = false;
                break;
            }
        }
        // if current number is prime, add it to the array
        if(isPrime) {
            val->primes.push_back(i);
        }
    }
}

// DAM1


class valsDAM{
    public:
    // array to store primes found by each thread
    vector<int> primes;
};

// no. of digits, no. of threads, current number to check for prime
int curr = 1;

// mutex for updating curr
sem_t mtx;

// Prime finding function for each thread
void getPrimesDAM(valsDAM* val) {
    int i, end = pow(10, n);
    while(true) {
        // get current number to check for prime
        sem_wait(&mtx);
        i = curr;
        // += 2 to skip even numbers
        curr+=2;
        sem_post(&mtx);
        // if current number is greater than end, return
        if(i>end) {
            return;
        }
        bool isPrime = true;
        if(i==1) {
            isPrime = false;
        }
        // check if current number is prime in sqrt(i) time
        for(int j=2; j*j<=i; j++) {
            if(i%j == 0) {
                isPrime = false;
                break;
            }
        }
        // if current number is prime, add it to the array
        if(isPrime) {
            val->primes.push_back(i);
        }
    }
}

int main(int argc, char *argv[]) {
    // Read input from file
    FILE* f_in = fopen(argv[1], "r");
    fscanf(f_in,"%d %d",&n,&m);
    fclose(f_in);

    // SAM

    // Initialize vector of vals
    vector<valsSAM> vSAM(m);

    // Initialize vector of threads
    vector<thread> tSAM;

    // Initialize timer
    struct timeval beginSAM, endSAM;
    gettimeofday(&beginSAM, 0);

    // Create m threads
    for(int i=0; i<m; i++) {
        vSAM[i].start = 2*i+1;
        tSAM.push_back(thread(getPrimesSAM,&vSAM[i]));
    }

    // Wait for all threads to finish and join
    for(int i=0; i<m; i++) {
        tSAM[i].join();
    }

    // get end time
    gettimeofday(&endSAM, 0);
    double elapsedSAM = (1000000 * (endSAM.tv_sec - beginSAM.tv_sec) + (endSAM.tv_usec - beginSAM.tv_usec))/(1000000.0);

    // vector to store all primes
    vector<int> primesSAM;
    
    primesSAM.push_back(2);

    // Merge all the primes
    for(int i=0; i<m; i++) {
        for(int j=0; j<vSAM[i].primes.size(); j++) {
            primesSAM.push_back(vSAM[i].primes[j]);
        }
    }

    // Sort the primes
    sort(primesSAM.begin(), primesSAM.end());

    // Open output file
    FILE* f_out = fopen("Primes-SAM1.txt", "w");

    // Write output to file
    for(int i=0; i<primesSAM.size(); i++) {
        fprintf(f_out, "%d ", primesSAM[i]);
    }
    fprintf(f_out, "\n");

    fclose(f_out);

    // DAM1

    // Initialize mutex
    sem_init(&mtx,0,1);

    // Initialize vector of vals
    vector<valsDAM> vDAM(m);

    // vector to store threads
    vector<thread> tDAM;

    // get start time
    struct timeval beginDAM, endDAM;
    gettimeofday(&beginDAM, 0);

    // Create m threads
    for(int i=0; i<m; i++) {
        tDAM.push_back(thread(getPrimesDAM,&vDAM[i]));
    }

    // Wait for all threads to finish and join
    for(int i=0; i<m; i++) {
        tDAM[i].join();
    }

    // get end time
    gettimeofday(&endDAM, 0);
    double elapsedDAM = (1000000 * (endDAM.tv_sec - beginDAM.tv_sec) + (endDAM.tv_usec - beginDAM.tv_usec))/(1000000.0);

    // vector to store all primes
    vector<int> primesDAM;
    
    primesDAM.push_back(2);

    // Merge all the primes
    for(int i=0; i<m; i++) {
        for(int j=0; j<vDAM[i].primes.size(); j++) {
            primesDAM.push_back(vDAM[i].primes[j]);
        }
    }

    // Sort the primes
    sort(primesDAM.begin(), primesDAM.end());

    // Open output file
    f_out = fopen("Primes-DAM1.txt", "w");

    // Write output to file
    for(int i=0; i<primesDAM.size(); i++) {
        fprintf(f_out, "%d ", primesDAM[i]);
    }
    fprintf(f_out, "\n");

    fclose(f_out);

    // Print time taken
    f_out = fopen("Times.txt", "w");
    // in seconds
    fprintf(f_out, "%lf %lf\n", elapsedDAM, elapsedSAM);
    fclose(f_out);

    return 0;
}
