#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

// to store log messages
vector<string> logs(100000);
atomic<int> logIndex(0);

// input parameters
int nW, nS, capacity, k;
double lambdaW, lambdaS;
bool term = false;

// class to store value, stamp and id
class stampedValue {
    public:
    short int value; // value
    short int stamp; // last update time
    short int id; // thread id
    short int temp; // temporary value, each short int is of size 2 bytes. atomic library only supports structures with total 8 bytes
    stampedValue() {
        this->value = 0;
        this->stamp = 0;
        this->id = 0;
    }
    stampedValue(int value, int stamp, int id) {
        this->value = value;
        this->stamp = stamp;
        this->id = id;
    }
};

// to check if two vectors of stampedValues are equal
bool equalStamps(vector<stampedValue> a, vector<stampedValue> b) {
    for(int j=0; j<a.size(); j++) {
        if(a[j].stamp != b[j].stamp || a[j].id != b[j].id) {
            return false;
        }
    }
    return true;
}

class WFS {
    // shared array
    atomic<stampedValue> a_table[100];
    // to store snapshots for each thread
    vector<vector<int>> helpsnaps;
    public:
    WFS(int init) {
        // initialize array
        for(int j=0; j<capacity; j++) {
            a_table[j].store(stampedValue(init, 0, 0), std::memory_order_relaxed);
        }
        helpsnaps.resize(100);
    }
    void update(int value, int id, int tid) {
        // update value at index id by thread tid
        stampedValue oldValue = a_table[id].load();
        stampedValue newValue = stampedValue(value, (oldValue.stamp)+1, tid);
        a_table[id].store(newValue);
        // take a snapshot after update
        helpsnaps[tid-1] = scan();
    }
    // collect values from array
    vector<stampedValue> collect() {
        vector<stampedValue> copy(capacity);
        for(int j=0; j<capacity; j++) {
            copy[j] = a_table[j].load();
        }
        return copy;
    }
    // scan the array
    vector<int> scan() {
        vector<stampedValue> oldCopy, newCopy;
        bool moved[100] = {false};
        oldCopy = collect();
        // keep scanning until get a consistent snapshot
        while(true) {
            newCopy = collect();
            bool flag = true;
            // check if two snapshots are equal
            for(int i=0; i<capacity; i++) {
                if(oldCopy[i].stamp != newCopy[i].stamp) {
                    int w = newCopy[i].id;
                    // check if thread has already moved
                    if(moved[w]) {
                        return helpsnaps[w];
                    }
                    else {
                        // can help thread
                        moved[w] = true;
                        oldCopy = newCopy;
                        flag = false;
                        break;
                    }
                }
            }
            // unequal snapshots and no thread to help, hence continue scanning
            if(!flag) {
                continue;
            }
            vector<int> result(capacity);
            for(int j=0; j<capacity; j++) {
                result[j] = newCopy[j].value;
            }
            return result;
        }
    }
};

WFS mrmwSnapObj(0);

// writer thread function
void writer(int tid, vector<long>* wrTime) {
    int v, t, l;
    struct timeval begin, end;
    // random number generators
    default_random_engine random_number_generator(tid+time(NULL));
    exponential_distribution<double> expRandObj(1.0/lambdaW);
    uniform_int_distribution<int> unifRandObj(0, capacity-1);
    uniform_real_distribution<double> unifRandObj2(0, 10000);
    // keep writing until termination
    while(!term) {
        // generate random value, location
        v = unifRandObj2(random_number_generator);
        l = unifRandObj(random_number_generator);

        // get update start time
        time_t upTime = time(NULL);
        tm* uT = localtime(&upTime);
        gettimeofday(&begin, 0);
        // update value
        mrmwSnapObj.update(v, l, tid);
        gettimeofday(&end, 0);

        // push update time to vector
        (*wrTime).push_back(1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec));

        // push log message
        int ind = logIndex.fetch_add(1);
        logs[ind] = "Thr" + to_string(tid) + "'s write of " + to_string(v) + " on location " + to_string(l+1) + " at " + to_string(uT->tm_hour) + ":" + to_string(uT->tm_min) + ":" + to_string(uT->tm_sec);

        // sleep for exponential time
        t = expRandObj(random_number_generator);
        usleep(t*100000);
    }
}

void snapshot(int tid, vector<long>* snTime) {
    int t;
    struct timeval begin, end;
    // random number generators
    default_random_engine random_number_generator(tid+time(NULL));
    exponential_distribution<double> expRandObj(lambdaS);
    // perform k snapshots
    for(int i=0; i<k; i++) {
        gettimeofday(&begin, 0);
        // scan the array
        vector<int> result = mrmwSnapObj.scan();
        gettimeofday(&end, 0);

        // push snapshot time to vector
        (*snTime).push_back(1000000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec));

        // get snapshot time
        time_t ssTime = time(NULL);
        tm* sT = localtime(&ssTime);

        // push log message
        int ind = logIndex.fetch_add(1);
        string s = "Snapshot Thr" + to_string(tid) + "'s snapshot: ";
        for(int j=0; j<result.size(); j++) {
            s += "l" + to_string(j+1) + "-" + to_string(result[j]) + " ";
        }
        s += "which finished at " + to_string(sT->tm_hour) + ":" + to_string(sT->tm_min) + ":" + to_string(sT->tm_sec);

        logs[ind] = s;

        // sleep for exponential time
        t = expRandObj(random_number_generator);
        usleep(t*100000);
    }
}

// compute statistics
void computeStats(vector<vector<long>> snTime, vector<vector<long>> wrTime) {
    int w = 0;
    double avgTimeS = 0, avgTimeW = 0, avgTime = 0;

    // compute average time for snapshots
    for(int i=0; i<nS; i++) {
        for(int j=0; j<snTime[i].size(); j++) {
            avgTimeS += snTime[i][j];
        }
    }

    avgTimeS /= (nS*k*1.0);

    // compute average time for writes and number of writes
    for(int i=0; i<nW; i++) {
        for(int j=0; j<wrTime[i].size(); j++) {
            avgTimeW += wrTime[i][j];
            w++;
        }
    }

    avgTimeW /= (w*1.0);

    // compute average time for snapshots and writes
    avgTime = (nS*k*avgTimeS + w*avgTimeW)/(nS*k + w*1.0);

    cout << fixed << setprecision(10);
    cout << "Average time taken to complete " << nS*k << " snapshots by all the threads: " << avgTimeS << " \xC2\xB5s" << endl;
    cout << "Average time taken to complete " << w << " writes by all the threads: " << avgTimeW << " \xC2\xB5s" << endl;
    cout << "Average time taken to complete " << nS*k << " snapshots and " << w << " writes by all the threads: " << avgTime << " \xC2\xB5s" << endl;

    long mxS = 0, mxW = 0, mx = 0;

    // compute maximum time for snapshots and writes
    for(int i=0; i<nS; i++) {
        for(int j=0; j<snTime[i].size(); j++) {
            mxS = max(mxS, snTime[i][j]);
        }
    }

    for(int i=0; i<nW; i++) {
        for(int j=0; j<wrTime[i].size(); j++) {
            mxW = max(mxW, wrTime[i][j]);
        }
    }

    mx = max(mxS, mxW);

    cout << "Maximum time taken to complete " << nS*k << " snapshots by all the threads: " << mxS << " \xC2\xB5s" << endl;
    cout << "Maximum time taken to complete " << w << " writes by all the threads: " << mxW << " \xC2\xB5s" << endl;
    cout << "Maximum time taken to complete " << nS*k << " snapshots and " << w << " writes by all the threads: " << mx << " \xC2\xB5s" << endl;
}

int main(int argc, char *argv[]) {

    // Read input from file
    FILE* f_in = fopen(argv[1], "r");
    fscanf(f_in,"%d %d %d %lf %lf %d",&nW, &nS, &capacity, &lambdaW, &lambdaS, &k);
    fclose(f_in);

    // Time taken by each thread
    vector<vector<long>> snTime(nS), wrTime(nW);

    // Create threads
    vector<thread> threadsW;
    for(int i=0; i<nW; i++) {
        threadsW.push_back(thread(writer, i+1, &wrTime[i]));
    }

    vector<thread> threadsS;
    for(int i=0; i<nS; i++) {
        threadsS.push_back(thread(snapshot, i+1, &snTime[i]));
    }

    // Wait for threads to finish
    for(int i=0; i<nS; i++) {
        threadsS[i].join();
    }

    term = true;

    for(int i=0; i<nW; i++) {
        threadsW[i].join();
    }

    // log file
    FILE *f_out = fopen("wfs-out.txt", "w");

    for(int i=0; i<logIndex; i++) {
        fprintf(f_out, "%s\n", logs[i].c_str());
    }

    fclose(f_out);

    computeStats(snTime, wrTime);

    return 0;
}