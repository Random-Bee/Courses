#include "K-MVTO-CO21BTECH11002.h"
#include <unistd.h>
using namespace std;
using namespace chrono;
typedef long long ll;

ll n, m, totTrans, numIters, constVal, lambda;
double readRatio;

vector<ll> commitTime, aborts;

void getStats() {
    long double totWaitTime = 0, totAborts = 0;

    for(ll i=0; i<n; i++) {
        totWaitTime += commitTime[i];
        totAborts += aborts[i];
    }

    cout << setprecision(15);
    cout << "Average time to commit: " << totWaitTime/totTrans << " nanoseconds\n";
    cout << "Average number of aborts: " << totAborts/totTrans << "\n";
}

void updtMem(ll tid) {
    // exponential distribution with mean lambda
    exponential_distribution<double> expDist(1.0/lambda);

    bool status = 0; // 0 for aborted, 1 for committed
    ll abortCnt = 0;

    ll startTime, endTime, currTime, totTime = 0;

    // Some threads may have to do 1 more transaction than others
    ll numTrans = totTrans/n + (tid < totTrans%n ? 1 : 0);

    for(ll curTrans=0; curTrans<numTrans; curTrans++) {
        status = 0;

        startTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();

        // for logging
        string s;

        bool read_only = (getRand(1, 100) <= readRatio*100);

        do {
            Transaction* t = begin_trans();

            ll transId = t->id;
            
            ll locVal;

            // boolean to check if the transaction is aborted early due to some late read
            bool aborted = 0;

            for(ll i=0; i<numIters; i++) {
                ll randInd = getRand(0, m);

                ll randVal = getRand(0, constVal);

                // transaction t reads value at index randInd into locVal
                bool read_status = read(t, randInd, locVal);

                currTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();

                s = "Thread id " + to_string(tid) + " Transaction id " + to_string(transId) + " reads from " + to_string(randInd) + " a value " + to_string(locVal) + " at time " + to_string(currTime);
                log(s);

                // If the read was late, then the transaction should abort
                if(!read_status) {
                    aborted = 1;
                    break;
                }

                // if the transaction is not read-only, only then it can write
                if(!read_only) {
                    locVal += randVal;

                    // request to write back to the shared memory
                    write(t, randInd, locVal);

                    currTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();

                    s = "Thread id " + to_string(tid) + " Transaction id " + to_string(transId) + " writes to " + to_string(randInd) + " a value " + to_string(locVal) + " at time " + to_string(currTime);
                    log(s);
                }

                double randTime = expDist(rng);
                // sleep for randTime milliseconds
                usleep(randTime*1000); // usleep takes time in microseconds. So multiply by 1000 to convert to microseconds
            }

            // All the operations are done, try to commit the transaction
            status = tryCommit(t, aborted);

            currTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();

            s = "Thread id " + to_string(tid) + " Transaction id " + to_string(transId) + " tryCommits with result " + (status ? "committed" : "aborted") + " at time " + to_string(currTime);
            log(s);
            
            if(status != 1) {
                abortCnt++;
            }
        }
        while(status != 1);

        endTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();

        ll timeTaken = endTime - startTime;

        totTime += timeTaken;
    }

    commitTime[tid] = totTime;
    aborts[tid] = abortCnt;
}

int main() {
    FILE *f = fopen("inp-params.txt", "r");

    if(f == NULL) {
        cout << "File not found\n";
        return 0;
    }

    fscanf(f, "%lld %lld %lld %lld %lld %lld %lf", &n, &m, &totTrans, &constVal, &lambda, &numIters, &readRatio);

    fclose(f);

    initDatabase(m);

    logFile = fopen("K-MVTO-log.txt", "w");

    commitTime.resize(n, 0);
    aborts.resize(n, 0);

    vector<thread> threads(n);

    // create n threads
    for(int i = 0; i < n; i++) {
        threads[i] = thread(updtMem, i);
    }

    // wait for all threads to finish
    for(int i = 0; i < n; i++) {
        threads[i].join();
    }

    fclose(logFile);

    getStats();
}