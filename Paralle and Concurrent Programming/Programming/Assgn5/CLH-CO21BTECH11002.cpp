#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;
string name = "CLH";

FILE *f_out;

// input parameters
int n, k;
double lambda1, lambda2;

// class to store state of lock, volatile to prevent compiler optimization
class QNode {
    public:
    volatile bool locked = false;
};

// CLH lock object
class CLHLock {
    // shared array
    atomic<QNode*> tail;
    public:
    CLHLock() {
        QNode qnode;
        tail.store(&qnode);
    }
    // lock function
    void lock(atomic<QNode*>& myNode, atomic<QNode*>& myPred) {
        // get pointer to myNode
        QNode *qnode = myNode.load();
        // set locked to true
        (*qnode).locked = true;
        // get pointer to predecessor from tail
        QNode *pred = tail.exchange(qnode);
        // store predecessor in myPred
        myPred = pred;
        // spin until predecessor unlocks
        while((*pred).locked) {}
    }
    void unlock(atomic<QNode*>& myNode, atomic<QNode*>& myPred) {
        // get pointer to myNode
        QNode *qnode = myNode.load();
        // set locked to false, so that successor can enter CS
        (*qnode).locked = false;
        // recycle successor's node to use again
        myNode.store(myPred.load());
    }
};

CLHLock cLock;

// function to test CS
void testCS(int tid, long long* csEnterTime) {
    // each thread has its own node and predecessor pointers
    atomic<QNode*> myNode, myPred;
    // initialize node with new QNode
    myNode.store(new QNode());
    myPred.store(NULL);

    struct timeval begin, end;
    // random number generator
    default_random_engine random_number_generator(tid+time(NULL));
    exponential_distribution<double> expRandObj1(100.0/lambda1);
    exponential_distribution<double> expRandObj2(100.0/lambda2);

    // enter CS k times
    for(int i=1; i<=k; i++) {

        // get time of request to enter CS
        time_t reqEnterTime = time(NULL);
        gettimeofday(&begin, NULL);
        fprintf(f_out, "%dth CS entry request at %lld by thread %d\n", i, reqEnterTime, tid);

        // obtain lock
        cLock.lock(myNode, myPred);

        // get time of actual entry to CS
        time_t actEnterTime = time(NULL);
        gettimeofday(&end, NULL);
        fprintf(f_out, "%dth CS entry at %lld by thread %d\n", i, actEnterTime, tid);

        // add time taken to enter CS
        *csEnterTime += (end.tv_sec - begin.tv_sec)*1000000 + (end.tv_usec - begin.tv_usec);

        // sleep for exponential time
        double t1 = expRandObj1(random_number_generator);
        sleep(t1);

        // get time of request to exit CS
        time_t reqExitTime = time(NULL);
        fprintf(f_out, "%dth CS exit request at %lld by thread %d\n", i, reqExitTime, tid);

        // release lock
        cLock.unlock(myNode, myPred);

        // get time of actual exit from CS
        time_t actExitTime = time(NULL);
        fprintf(f_out, "%dth CS exit at %lld by thread %d\n", i, actExitTime, tid);

        // sleep for exponential time
        double t2 = expRandObj2(random_number_generator);
        sleep(t2);
    }
}

int main(int argc, char *argv[]) {

    // File handling
    if(argc != 2) {
        cout << "Usage: ./CLH-CO21BTECH11002.out <input_file>" << endl;
        return 0;
    }

    // Read input from file
    FILE* f_in = fopen(argv[1], "r");

    if(f_in == NULL) {
        cout << "Error opening input file" << endl;
        return 0;
    }

    fscanf(f_in,"%d %d %d %lf %lf %d",&n, &k, &lambda1, &lambda2);
    fclose(f_in);

    // Time taken by each thread
    vector<long long> csEnterTimes(n, 0);

    // log file
    f_out = fopen("logCLH.txt", "w");

    // Create threads
    vector<thread> threads;

    // get start time
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    time_t startTime = time(NULL);

    // start threads
    for(int i=0; i<n; i++) {
        threads.push_back(thread(testCS, i+1, &csEnterTimes[i]));
    }

    // Wait for threads to finish
    for(int i=0; i<n; i++) {
        threads[i].join();
    }

    // get end time
    gettimeofday(&end, NULL);
    time_t endTime = time(NULL);

    fclose(f_out);

    // calculate average time taken by each thread and throughput
    double averageTime = abs(((end.tv_sec - begin.tv_sec)*1000000 + (end.tv_usec - begin.tv_usec))/(1000000.0*n*k));
    double throughput = 1.0/averageTime;
    double avgCSEnterTime = accumulate(csEnterTimes.begin(), csEnterTimes.end(), 0.0)/(n*k);

    // print results
    cout << "Throughput: " << throughput << endl;
    cout << "Average time taken to enter CS: " << avgCSEnterTime << " \xC2\xB5s" << endl;

    return 0;
}