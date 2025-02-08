#include <bits/stdc++.h>
using namespace std;

// Total waiting time and worst waiting time
time_t total=0;
double worst=0;

// Pointer to log file
FILE* f_out;

// Class to store values of k, n, lambda1, lambda2 and thread id
class vals {
    public:
    int k;
    int n;
    double lam1;
    double lam2;
    int tid;
};

// Vector to store waiting status of threads
vector<int> waiting;

// atomic lock for BCAS
atomic<int> lockBCAS(0);

// Function to test critical section
void testCS(vals val) {
    int i,j;
    double t1,t2;

    // Random number generator for generating time
    default_random_engine random_number_generator;

    // Iterate for number of times thread wants to enter CS
    for(i=1;i<=val.k;i++) {

        // requested entry into CS
        time_t reqEnterTime = time(NULL);
        tm* req_now = localtime(&reqEnterTime);

        // Print into log file
        if(i==1) {
            fprintf(f_out,"%dst CS Requested at %d:%d:%d by Thread %d\n", i, req_now->tm_hour, req_now->tm_min, req_now->tm_sec, val.tid);
        }
        else if(i==2) {
            fprintf(f_out,"%dnd CS Requested at %d:%d:%d by Thread %d\n", i, req_now->tm_hour, req_now->tm_min, req_now->tm_sec, val.tid);
        }
        else if(i==3) {
            fprintf(f_out,"%drd CS Requested at %d:%d:%d by Thread %d\n", i, req_now->tm_hour, req_now->tm_min, req_now->tm_sec, val.tid);
        }
        else {
            fprintf(f_out,"%dth CS Requested at %d:%d:%d by Thread %d\n", i, req_now->tm_hour, req_now->tm_min, req_now->tm_sec, val.tid);
        }

        // wait while some other thread is in CS
        waiting[val.tid-1] = 1;
        int expected = 0, new_val = 1, key=1;
        
        while(waiting[val.tid-1] && key==1){
            expected = 0; new_val = 1;
            if(atomic_compare_exchange_strong(&lockBCAS, &expected, new_val)) {
                key = 0;
            }
        }

        waiting[val.tid-1] = 0;

        // entered CS
        time_t actEnterTime = time(NULL);
        tm* act_now = localtime(&actEnterTime);

        // Add current waiting time to total time
        total += actEnterTime - reqEnterTime;
        // Update worst waiting time
        worst = max(worst,1.0*actEnterTime - 1.0*reqEnterTime);

        // Print into log file
        if(i==1) {
            fprintf(f_out,"%dst CS Entered at %d:%d:%d by Thread %d\n", i, act_now->tm_hour, act_now->tm_min, act_now->tm_sec, val.tid);
        }
        else if(i==2) {
            fprintf(f_out,"%dnd CS Entered at %d:%d:%d by Thread %d\n", i, act_now->tm_hour, act_now->tm_min, act_now->tm_sec, val.tid);
        }
        else if(i==3) {
            fprintf(f_out,"%drd CS Entered at %d:%d:%d by Thread %d\n", i, act_now->tm_hour, act_now->tm_min, act_now->tm_sec, val.tid);
        }
        else {
            fprintf(f_out,"%dth CS Entered at %d:%d:%d by Thread %d\n", i, act_now->tm_hour, act_now->tm_min, act_now->tm_sec, val.tid);
        }

        // Perform CS work
        exponential_distribution<double> exp1(1.0/val.lam1);
        t1 = exp1(random_number_generator);
        sleep(t1);

        // exited CS
        time_t actExitTime = time(NULL);
        tm* actExit_now = localtime(&actExitTime);

        // Print into log file
        if(i==1) {
            fprintf(f_out,"%dst CS Exited at %d:%d:%d by Thread %d\n", i, actExit_now->tm_hour, actExit_now->tm_min, actExit_now->tm_sec, val.tid);
        }
        else if(i==2) {
            fprintf(f_out,"%dnd CS Exited at %d:%d:%d by Thread %d\n", i, actExit_now->tm_hour, actExit_now->tm_min, actExit_now->tm_sec, val.tid);
        }
        else if(i==3) {
            fprintf(f_out,"%drd CS Exited at %d:%d:%d by Thread %d\n", i, actExit_now->tm_hour, actExit_now->tm_min, actExit_now->tm_sec, val.tid);
        }
        else {
            fprintf(f_out,"%dth CS Exited at %d:%d:%d by Thread %d\n", i, actExit_now->tm_hour, actExit_now->tm_min, actExit_now->tm_sec, val.tid);
        }

        // Wait until no other thread is waiting before making another request
        j = val.tid % val.n;
        while ((j != val.tid-1) && !waiting[j]) {
            j = (j + 1) % val.n;
        }
        if (j == val.tid-1) {
            lockBCAS = 0;
        }
        else {
            waiting[j] = false;
        }

        // Perform remainder section work
        exponential_distribution<double> exp2(1.0/val.lam2);
        t2 = exp2(random_number_generator);
        sleep(t2);
    }
    return;
}

int main(int argc, char* argv[]) {

    int n,k;
    double lam1,lam2;
    
    // Read input from file
    FILE *f1;
    f1 = fopen(argv[1], "r");
    fscanf(f1,"%d %d %lf %lf",&n,&k,&lam1,&lam2);
    fclose(f1);

    // Resize waiting vector to number of threads
    waiting.resize(n,1);

    // Create n data holders
    vals d[n];
    for(int i=0;i<n;i++) {
        d[i].k = k;
        d[i].n = n;
        d[i].lam1 = lam1;
        d[i].lam2 = lam2;
        d[i].tid = i+1;
    }

    // Open output file
    f_out = fopen("Output-BCAS.txt","w");
    fprintf(f_out,"Bounded CAS ME Output\n");

    // Create n threads
    vector<thread> t;
    for(int i=0;i<n;i++) {
        t.emplace_back(testCS,d[i]);
    }
    // Wait for all threads to finish and join
    for(int i=0;i<n;i++) {
        t[i].join();
    }

    // Close output file
    fclose(f_out);

    double avg = total*1.0/(n*k*1.0);

    // Print results
    cout << "Average waiting time: " << avg << " seconds" << endl;
    cout << "Worst waiting time: " << worst << " seconds" << endl;

    return 0;
}