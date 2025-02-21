#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

class AtomicMRMWRegister {
    vector<pair<int, int>> arr;
    public:
    AtomicMRMWRegister(int n) {
        arr.resize(n, {0, 0});
    }
    void write(int v, int id) {
        pair<int,int> p = {0, 0};
        // find the most recent value
        for(int i=0; i<arr.size(); i++) {
            if(p.second < arr[i].second) {
                p = arr[i];
            }
        }
        // update the most recent value
        arr[id] = {v, p.second+1};
    }
    int read() {
        pair<int,int> p = {0, 0};
        // find the most recent value
        for(int i=0; i<arr.size(); i++) {
            if(p.second < arr[i].second) {
                p = arr[i];
            }
        }
        return p.first;
    }
} shVar(0);

atomic<int> shVar2(0);

FILE* f_out;

// For testing custom atomic register
void testAtomic(int tid, int k, double lambda, long* readTime, long* writeTime) {
    struct timeval begin, end;
    int lVar;
    
    // Initialize random number generator
    default_random_engine random_number_generator(tid+time(NULL));
    exponential_distribution<double> expRandObj(lambda);
    uniform_real_distribution<double> unifRandObj(0, 1);
    
    for(int i=1; i<=k; i++) {
        // get request time
        time_t reqTime = time(NULL);
        tm* rT = localtime(&reqTime);
        fprintf(f_out, "%dth action requested at %d:%d:%d by thread %d\n", i, rT->tm_hour, rT->tm_min, rT->tm_sec, tid);
        
        // action to be performed
        double p = unifRandObj(random_number_generator);
        
        // get start time
        gettimeofday(&begin, NULL);
        if(p<0.5) {
            lVar = shVar.read();
            // get end time
            gettimeofday(&end, NULL);
            fprintf(f_out, "Thread %d read %d\n", tid, lVar);
        }
        else {
            lVar = i*tid;
            shVar.write(lVar, tid);
            // get end time
            gettimeofday(&end, NULL);
            fprintf(f_out, "Thread %d wrote %d\n", tid, lVar);
        }


        time_t complTime = time(NULL);
        tm* cT = localtime(&complTime);
        if(p<0.5) {
            *readTime += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
        }
        else {
            *writeTime += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
        }
        // update log file
        fprintf(f_out, "%dth action completed at %d:%d:%d by thread %d\n", i, cT->tm_hour, cT->tm_min, cT->tm_sec, tid);

        // simulate other tasks
        double t = expRandObj(random_number_generator)*1000000; // time in microseconds
        usleep(t);
    }
}

// For testing in-built atomic register
void testAtomic2(int tid, int k, double lambda, long* readTime, long* writeTime) {
    struct timeval begin, end;
    int lVar;

    // Initialize random number generator
    default_random_engine random_number_generator(tid+time(NULL));
    exponential_distribution<double> expRandObj(lambda);
    uniform_real_distribution<double> unifRandObj(0, 1);
    
    for(int i=1; i<=(k); i++) {
        // get request time
        time_t reqTime = time(NULL);
        tm* rT = localtime(&reqTime);
        fprintf(f_out, "%dth action requested at %d:%d:%d by thread %d\n", i, rT->tm_hour, rT->tm_min, rT->tm_sec, tid);

        // action to be performed
        double p = unifRandObj(random_number_generator);
        // get start time
        gettimeofday(&begin, NULL);
        if(p<0.5) {
            lVar = shVar2.load();
            // get end time
            gettimeofday(&end, NULL);
            fprintf(f_out, "Thread %d read %d\n", tid, lVar);
        }
        else {
            lVar = i*tid;
            shVar2.store(lVar);
            // get end time
            gettimeofday(&end, NULL);
            fprintf(f_out, "Thread %d wrote %d\n", tid, lVar);
        }

        time_t complTime = time(NULL);
        tm* cT = localtime(&complTime);
        if(p<0.5) {
            *readTime += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
        }
        else {
            *writeTime += 1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec);
        }
        // update log file
        fprintf(f_out, "%dth action completed at %d:%d:%d by thread %d\n", i, cT->tm_hour, cT->tm_min, cT->tm_sec, tid);

        // simulate other tasks
        double t = expRandObj(random_number_generator)*1000000; // time in microseconds
        usleep(t);
    }
}

int main(int argc, char *argv[]) {
    int capacity, numOps;
    double lambda;

    // Read input from file
    FILE* f_in = fopen(argv[1], "r");
    fscanf(f_in,"%d %d %lf",&capacity, &numOps, &lambda);
    fclose(f_in);

    shVar = AtomicMRMWRegister(capacity);

    // For testing custom atomic register

    // Initialize threads
    vector<thread> threads;
    vector<long> readTimes(capacity, 0), writeTimes(capacity, 0);

    // Open output file
    f_out = fopen("logfile.txt", "w");

    for(int i=0; i<capacity; i++) {
        threads.push_back(thread(testAtomic, i+1, numOps, lambda, &readTimes[i], &writeTimes[i]));
    }

    // Wait for all threads to complete and join
    for(int i=0; i<capacity; i++) {
        threads[i].join();
    }

    fclose(f_out);

    // Compute average time taken by all the threads
    double avgTime = 0;

    for(int i=0; i<capacity; i++) {
        avgTime += readTimes[i] + writeTimes[i];
    }
    avgTime /= (capacity*numOps*1.0);

    cout << fixed << setprecision(15);
    cout << "Average time taken to complete numOps operations by all the threads using custom MRMW register: " << avgTime << " \xC2\xB5s" << endl;

    // For testing in-built atomic register

    // reset variables
    threads.clear(); fill(readTimes.begin(), readTimes.end(), 0); fill(writeTimes.begin(), writeTimes.end(), 0);

    f_out = fopen("logfile2.txt", "w");

    for(int i=0; i<capacity; i++) {
        threads.push_back(thread(testAtomic2, i+1, numOps, lambda, &readTimes[i], &writeTimes[i]));
    }

    // Wait for all threads to complete and join
    for(int i=0; i<capacity; i++) {
        threads[i].join();
    }

    fclose(f_out);

    // Calculate average time taken by all the threads
    avgTime = 0;

    for(int i=0; i<capacity; i++) {
        avgTime += readTimes[i] + writeTimes[i];
    }

    avgTime /= (capacity*numOps*1.0);

    cout << "Average time taken to complete numOps operations by all the threads using in-built MRMW register: " << avgTime << " \xC2\xB5s" << endl;
    
    return 0;
}