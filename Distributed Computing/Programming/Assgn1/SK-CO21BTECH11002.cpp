#include <bits/stdc++.h>
#include <mpi.h>
#include <sys/time.h>
#include <semaphore.h>
typedef long long ll;

using namespace std;

// pid - process id, np - number of processes + 1
int pid, np;

// n - number of processes, numMessages - number of messages to be sent by each process
int n, numMessages;
// lambda - exponential distribution parameter, thr - threshold for internal event
double lambda, thr;

// edges - adjacency list, clk - vector clock, LS - last sent, LU - last updated
vector<int> edges;
vector<int> clk, LS, LU;

// mutex to make sure only one process updates vector clock at a time
sem_t mtx;

// function to simulate send and internal work events
void doWork() {
    // random number generator
    default_random_engine generator(time(0) + pid);
    uniform_int_distribution<int> distribution(1, 100);
    uniform_real_distribution<double> distribution2(0, 1);
    exponential_distribution<double> distribution3(1.0/(2.0*lambda));
    // message count, internal event count, number of edges
    int i, msgCnt = 0, inCnt = 0, numEdges = edges.size();
    // log msg structure - event type, current size, pid, event id, dest, time, numEntries, vector clock => n+7 (in this order)
    // since log msg is not part of the algorithm, we will not consider its size
    int logMsg[n+7];
    // run the loop until numMessages messages are sent
    while(msgCnt < numMessages) {
        // generate a random number between 0 and 1
        double task = distribution2(generator);
        // if task < thr/(1+thr), perform internal event
        if(task < thr/(1+thr)) {
            time_t t = time(0);
            // obtain the mutex and update vector clock and last updated array
            sem_wait(&mtx);
            clk[pid-1]++;
            LU[pid-1] = clk[pid-1];
            sem_post(&mtx);
            inCnt++;
            // log the event
            logMsg[0] = 0; logMsg[1] = sizeof(clk)+sizeof(LS)+sizeof(LU); logMsg[2] = pid;
            logMsg[3] = inCnt; logMsg[4] = -1; logMsg[5] = t; logMsg[6] = 0;
            for(int i=0; i<n; i++) {
                logMsg[i+7] = clk[i];
            }
            // send to p0 for logging
            MPI_Send(&logMsg, n+7, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        // else, send a message to a random process
        else {
            // get a random destination from edges
            int dest = edges[distribution(generator)%(numEdges-1) + 1];
            time_t t = time(0);
            msgCnt++;
            // obtain the mutex and update vector clock and last updated array
            sem_wait(&mtx);
            clk[pid-1]++;
            LU[pid-1] = clk[pid-1];
            sem_post(&mtx);
            // create the message and log the event
            // msg structure - process id, message count/id, number of entries, vector clock => max n+3 (in this order)
            vector<int> message(2);
            message[0] = pid; message[1] = msgCnt;
            // send the message to the destination
            int numEn = 0;
            // get the elements of vector clock that needs to be sent
            for(int i=0; i<n; i++) {
                if(LU[i] > LS[dest-1]) {
                    // last two digits of message will be the process id
                    message.push_back(clk[i]*100 + i);
                    numEn++;
                }
            }
            // insert the number of entries at the beginning of the message
            message.insert(message.begin()+2, numEn);
            // update last sent array
            LS[dest-1] = clk[pid-1];
            // send the message to the destination
            MPI_Send(message.data(), message.size(), MPI_INT, dest, 0, MPI_COMM_WORLD);
            // send to p0 for logging
            for(int i=0; i<n; i++) {
                logMsg[i+7] = clk[i];
            }
            logMsg[0] = 1; logMsg[1] = sizeof(clk)+sizeof(message)+sizeof(LS)+sizeof(LU); logMsg[2] = pid;
            logMsg[3] = msgCnt; logMsg[4] = dest; logMsg[5] = t; logMsg[6] = numEn;
            MPI_Send(&logMsg, n+7, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        // sleep for a random amount of time
        double sleep = distribution3(generator);
        // usleep takes input in microseconds, so multiply by 10000
        usleep(sleep*10000);
    }
    // send a termination message to p0
    int message = -1;
    MPI_Send(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

void receiveMsgs() {
    while (1) {
        // log msg structure - event type, current size, pid, event id, dest, time, vector clock => n+6 (in this order)
        int message[n+3], logMsg[n+7];
        MPI_Recv(&message, n+3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // if message is -1, the message is a termination message, break the loop
        if(message[0]==-1) {
            break;
        }
        time_t t = time(0);
        // obtain the mutex and update vector clock
        sem_wait(&mtx);
        clk[pid-1]++;
        LU[pid-1] = clk[pid-1];
        for(int i=0; i<message[2]; i++) {
            // last two digits of message will be the process id
            int in = message[i+3]%100, val = message[i+3]/100;
            if(clk[in] < val) {
                clk[in] = val;
                LU[in] = clk[pid-1];
            }
        }
        sem_post(&mtx);
        // send to p0 for logging
        logMsg[0] = 2; logMsg[1] = sizeof(clk)+sizeof(message)+sizeof(LS)+sizeof(LU); logMsg[2] = pid;
        logMsg[3] = message[1]; logMsg[4] = message[0]; logMsg[5] = time(0); logMsg[6] = 0;
        for(int i=0; i<n; i++) {
            logMsg[i+7] = clk[i];
        }
        MPI_Send(&logMsg, n+7, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char *argv[]) {

    // open the file to read input parameters
    ifstream file;
    file.open("inp-params.txt");

    if(!file) {
        cout << "Error in opening the file" << endl;
        return 0;
    }

    file >> n >> lambda >> thr >> numMessages;

    // initialize vector clock
    clk.resize(n, 0);
    LS.resize(n, 0);
    LU.resize(n, 0);

    // initialize mutex
    sem_init(&mtx, 0, 1);

    // Initializes the MPI execution environment
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if(n+1!=np) {
        cout << "Number of processes should be equal to number of processes in the input file + 1" << endl;
        MPI_Finalize();
        return 0;
    }

    // root process, logs the events and sends termination message to all receivers once all messages are sent
    if(pid==0) {
        file.close();
        // open the file to write logs
        ofstream file1;
        file1.open("log-SK.txt");
        int cnt = 0, size = 0, cntEntries = 0;
        double avgEntries = 0;
        // run the loop until all processes have sent termination message
        while(cnt != n) {
            // log msg structure - event type, current size, pid, event id, dest, time, vector clock => n+6 (in this order)
            int message[n+7];
            MPI_Recv(&message, n+7, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // internal event
            if(message[0]==0) {
                file1 << "Process" << message[2] << " performs internal event e" << message[2] << "_" << message[3] << " at " << message[5] << ", vc: ";
                for(int i=0; i<n; i++) {
                    file1 << message[i+7] << " ";
                }
                file1 << endl;
                size = max(size, message[1]);
            }
            // send event
            else if(message[0]==1) {
                file1 << "Process" << message[2] << " sends message m" << message[2] << "_" << message[3] << " to process" << message[4] << " at " << message[5] << ", vc: ";
                for(int i=0; i<n; i++) {
                    file1 << message[i+7] << " ";
                }
                file1 << endl;
                size = max(size, message[1]);
                avgEntries += message[6];
                cntEntries++;
            }
            // receive event
            else if(message[0]==2) {
                file1 << "Process" << message[2] << " receives message m" << message[4] << "_" << message[3] << " from process" << message[4] << " at " << message[5] << ", vc: ";
                for(int i=0; i<n; i++) {
                    file1 << message[i+7] << " ";
                }
                file1 << endl;
                size = max(size, message[1]);
            }
            // termination message
            else {
                cnt++;
            }
        }
        // calculate average vector clock entries sent
        avgEntries /= cntEntries*1.0;
        file1 << endl;
        file1 << "Max space used (in bytes): " << size << endl;
        file1 << "Average vector clock entries sent: " << avgEntries << endl;

        file1.close();

        cout << "Max space used (in bytes): " << size << endl;
        cout << "Average vector clock entries sent: " << avgEntries << endl;

        // send termination message to all processes
        int message = -1;
        for(int i=1; i<np; i++) {
            MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    // other processes, simulate the events
    else {
        // get pid'th line from file and store in edges
        string line;
        for(int i=0; i<pid; i++) {
            getline(file, line);
        }
        getline(file, line);
        stringstream ss(line);
        int x;
        while(ss >> x) {
            edges.push_back(x);
        }
        file.close();

        // create two threads, one for sending messages and internal events and the other for receiving messages
        thread threads[2];
        threads[0] = thread(doWork);
        threads[1] = thread(receiveMsgs);
        threads[0].join();
        threads[1].join();
    }

    // Terminate MPI execution environment
    MPI_Finalize();

    return 0;
}