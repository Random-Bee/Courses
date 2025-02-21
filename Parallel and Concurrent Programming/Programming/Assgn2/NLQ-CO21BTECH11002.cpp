#include <bits/stdc++.h>
#include <semaphore.h>
#include <sys/time.h>
using namespace std;

int n, numOps;
double rndLt, lambda;

class NLQ {
    atomic<int> tail = 0;
    atomic<int> arr[100000] = {};
    public:
    void enqueue(int x) {
        // atomically fetch and increment tail and enqueue x at the index
        int t = tail.fetch_add(1);
        arr[t].store(x);
    }
    int dequeue() {
        while(true) {
            // atomically fetch tail and dequeue from the first non-zero index
            int t = tail.load();
            for(int i=0; i<t; i++) {
                // atomically fetch and dequeue from the index, sets value at index to 0
                int V = arr[i].fetch_and(0);
                if(V != 0) {
                    return V;
                }
            }
        }
    }
};

NLQ Q;

void testThread(int* tid, long* t, long* enqTime, long* deqTime, long* numEnq, long* numDeq) {
    struct timeval begin, end;
    default_random_engine random_number_generator(*tid+time(NULL));
    exponential_distribution<double> expRandObj(lambda);
    uniform_int_distribution<int> unifRandObj(1, 1000000);
    uniform_real_distribution<double> unifRandObj2(0, 1);
    int V;
    for(int i=0; i<numOps; i++) {
        // cout << i << endl;
        double p = unifRandObj2(random_number_generator);
        if(p < rndLt) {
            V = unifRandObj(random_number_generator);
            gettimeofday(&begin, 0);
            // Enqueue
            Q.enqueue(V);
            gettimeofday(&end, 0);
            // Update stats
            (*numEnq)++;
            *enqTime += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
        }
        else {
            gettimeofday(&begin, 0);
            // Dequeue
            V = Q.dequeue();
            gettimeofday(&end, 0);
            (*numDeq)++;
            // Update stats
            *deqTime += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
        }
        // Update total time
        *t += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
        // Sleep for random time
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

    FILE* f_out = fopen("NLQ-out.txt", "w");
    fprintf(f_out, "Average time taken to complete numOps operations by all the threads: %lf \xC2\xB5s\n", total*1.0/n);
    fclose(f_out);
}

int main(int argc, char *argv[]) {
    // Read input from file
    FILE* f_in = fopen(argv[1], "r");
    fscanf(f_in,"%d %d %lf %lf",&n,&numOps, &rndLt, &lambda);
    fclose(f_in);

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
