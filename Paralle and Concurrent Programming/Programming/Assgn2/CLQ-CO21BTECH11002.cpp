#include <bits/stdc++.h>
#include <semaphore.h>
#include <sys/time.h>
using namespace std;

int n, numOps;
double rndLt, lambda;

sem_t Lock;

class CLQ {
    int head = 0, tail = 0, size;
    vector<int> arr;
    public:
    CLQ() {
        size = n*numOps;
        arr.resize(size);
    }
    void enqueue(int x) {
        // wait for lock
        sem_wait(&Lock);
        // check if queue is full
        if(tail-head == size) {
            // release lock
            sem_post(&Lock);
            return;
        }
        // enqueue
        arr[tail%size] = x;
        tail++;
        // release lock
        sem_post(&Lock);
    }
    int dequeue() {
        // wait for lock
        sem_wait(&Lock);
        // check if queue is empty
        if(tail-head == 0) {
            // release lock
            sem_post(&Lock);
            return -1;
        }
        // dequeue
        int x = arr[head%size];
        head++;
        // release lock
        sem_post(&Lock);
        return x;
    }
};

CLQ Q;

void testThread(int* tid, long* t, long* enqTime, long* deqTime, long* numEnq, long* numDeq) {
    struct timeval begin, end;
    // Initialize random number generator
    default_random_engine random_number_generator(*tid+time(NULL));
    exponential_distribution<double> expRandObj(lambda);
    uniform_int_distribution<int> unifRandObj(1, 1000000);
    int V;
    for(int i=0; i<numOps; i++) {
        // get random number
        double p = expRandObj(random_number_generator);
        if(p < rndLt) {
            V = unifRandObj(random_number_generator);
            gettimeofday(&begin, 0);
            // enqueue
            Q.enqueue(V);
            gettimeofday(&end, 0);
            // update enqueue time
            *enqTime += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
            (*numEnq)++;
        }
        else {
            gettimeofday(&begin, 0);
            // dequeue
            V = Q.dequeue();
            gettimeofday(&end, 0);
            // update dequeue time
            *deqTime += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
            (*numDeq)++;
        }
        // update total time
        *t += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
        // simulate exponential delay
        double sleepTime = expRandObj(random_number_generator);
        usleep(sleepTime*100000);
    }
}

void computeStats(vector<long> &enqTime, vector<long> &deqTime, vector<long> &thTime, vector<long> &numEnq, vector<long> &numDeq) {
    long enq = 0, deq = 0, total = 0, nEnq = 0, nDeq = 0;

    for(int i=0; i<n; i++) {
        enq += enqTime[i];
        deq += deqTime[i];
        total += thTime[i];
        nEnq += numEnq[i];
        nDeq += numDeq[i];
    }

    cout << fixed << setprecision(6);
    cout << "Average time taken to complete numOps operations by all the threads: " << total*1.0/(n*numOps) << " \xC2\xB5s" << endl;
    cout << "Average time taken for enqueue operations: " << enq*1.0/nEnq << " \xC2\xB5s" << endl;
    cout << "Average time taken for dequeue operations: " << deq*1.0/nDeq << " \xC2\xB5s" << endl;
    cout << "Throughput: " << numOps*n*1.0*1000000/total << endl;

    FILE* f_out = fopen("CLQ-out.txt", "w");
    fprintf(f_out, "Average time taken to complete numOps operations by all the threads: %lf \xC2\xB5s\n", total*1.0/n);
    fclose(f_out);
}

int main(int argc, char *argv[]) {
    // Read input from file
    FILE* f_in = fopen(argv[1], "r");
    fscanf(f_in,"%d %d %lf %lf",&n,&numOps, &rndLt, &lambda);
    fclose(f_in);

    Q = CLQ();

    // Initialize lock
    sem_init(&Lock, 0, 1);

    // Initialize vector of time taken by threads
    vector<long> enqTime(n,0), deqTime(n,0), thTime(n,0), numEnq(n,0), numDeq(n,0);

    // Initialize vector of threads
    vector<thread> ts;

    // Create n threads
    for(int i=0; i<n; i++) {
        ts.push_back(thread(testThread, &i, &thTime[i], &enqTime[i], &deqTime[i], &numEnq[i], &numDeq[i]));
    }

    // Wait for all threads to finish and join
    for(int i=0; i<n; i++) {
        ts[i].join();
    }

    computeStats(enqTime, deqTime, thTime, numEnq, numDeq);

    return 0;
}
