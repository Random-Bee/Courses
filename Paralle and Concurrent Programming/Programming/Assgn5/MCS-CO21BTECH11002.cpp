#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;
string name = "MCS";

FILE *f_out;

// input parameters
int n, k;
double lambda1, lambda2;

// class to store state of lock, volatile to prevent compiler optimization, also successor pointer
class QNode {
    public:
    volatile bool locked = false;
    volatile QNode* next = NULL;
};

// MCS lock object
class MCSLock {
    // shared array
    atomic<QNode*> tail;
    public:
    MCSLock() {
        tail.store(NULL);
    }
    // lock function
    void lock(atomic<QNode*>& myNode) {
        // get pointer to myNode
        QNode *qnode = myNode.load();
        // get pointer to predecessor from tail
        QNode *pred = tail.exchange(qnode);
        // if pred is null then its the first thread, so no need to wait
        if(pred!=NULL) {
            // set locked to true
            (*qnode).locked = true;
            // set successor pointer of predecessor to myNode
            (*pred).next = qnode;
            // spin until predecessor unlocks
            while((*qnode).locked) {}
        }
    }
    // unlock function
    void unlock(atomic<QNode*>& myNode) {
        // get pointer to myNode
        QNode *qnode = myNode.load();
        if((*qnode).next==NULL) {
            // if no successor, then check if tail is still myNode, if yes then return
            if(tail.compare_exchange_strong(qnode, NULL)) {
                return;
            }
            // spin until successor pointer is set
            while((*qnode).next==NULL) {}
        }
        // set successor's locked to false
        (*((*qnode).next)).locked = false;
        (*qnode).next = NULL;
    }
};

MCSLock mLock;

// function to test CS
void testCS(int tid, long long* csEnterTime) {
    // each thread has its own node and predecessor pointers
    atomic<QNode*> myNode;
    // initialize node with new QNode
    myNode.store(new QNode());

    struct timeval begin, end;
    // random number generator
    default_random_engine random_number_generator(tid+time(NULL));
    exponential_distribution<double> expRandObj1(10.0/lambda1);
    exponential_distribution<double> expRandObj2(10.0/lambda2);

    // enter CS k times
    for(int i=1; i<=k; i++) {

        // get time of request to enter CS
        time_t reqEnterTime = time(NULL);
        gettimeofday(&begin, NULL);
        fprintf(f_out, "%dth CS entry request at %lld by thread %d\n", i, reqEnterTime, tid);

        // obtain lock
        mLock.lock(myNode);

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
        mLock.unlock(myNode);

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
        cout << "Usage: ./MCS-CO21BTECH11002.out <input_file>" << endl;
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
    f_out = fopen("logMCS.txt", "w");

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
    double averageTime = abs(((begin.tv_sec - end.tv_sec)*1000000 + (begin.tv_usec - end.tv_usec))/(1000000.0*n*k));
    double throughput = 1.0/averageTime;
    double avgCSEnterTime = accumulate(csEnterTimes.begin(), csEnterTimes.end(), 0.0)/(n*k);

    // print results
    cout << "Throughput: " << throughput << endl;
    cout << "Average time taken to enter CS: " << avgCSEnterTime << " \xC2\xB5s" << endl;

    return 0;
}