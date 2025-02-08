#include <bits/stdc++.h>
#include <sys/wait.h>
#include <zmq.hpp>
#include <semaphore.h>
#include <sys/time.h>
using namespace std;

const string base_endpoint = "tcp://127.0.0.1:";

// Number of processes, number of iterations, exponential distribution parameters
int n, k;
double a, b;
// Custom process id
int id;
// Number of messages sent
int numMessages = 0;
// Total runtime
long long totalRuntime = 0;
// Total time spent in critical section/waiting for critical section
long long totalCS = 0;

// Process variables
class procVars{
    public:
    vector<int> quorum;
    // lockedOn - process on which the process is locked
    // first - lamport clock of the process, second - process id
    pair<int,int> lockedOn = make_pair(-1,-1);
    // queue of processes requesting to enter CS
    set<pair<int,int>> queue;
    // number of replies expected to enter CS, number of replies received
    int numRepliesExpected;
    int numRepliesReceived;
    // number of failed messages received, number of inquire messages received
    int numFailed, numInquire;
    // Current lamport clock
    int lamportClock;
    // Clock at which process requested to enter CS
    int csEntryClock;
    // Mutex for process variables
    sem_t eventMtx;
    
    procVars() {
        lamportClock = 0;
        sem_init(&eventMtx, 0, 1);
    }

    // request to enter critical section
    // Initialize the process variables, increment lamport clock
    void reqCS() {
        sem_wait(&eventMtx);
        numFailed = 0;
        numInquire = 0;
        numRepliesExpected = 0;
        numRepliesReceived = 0;
        numFailed = 0;
        numInquire = 0;
        lamportClock++;
        csEntryClock = lamportClock;
        // number of replies expected is the number of processes in quorum
        numRepliesExpected = quorum.size();
        sem_post(&eventMtx);
    }

    // check if process can enter critical section
    // Possible only if number of replies received is equal to number of replies expected
    bool canEnterCS() {
        sem_wait(&eventMtx);
        if(numRepliesReceived == numRepliesExpected) {
            sem_post(&eventMtx);
            return true;
        }
        sem_post(&eventMtx);
        return false;
    }

    // update lamport clock
    void updateClock(int receivedClock) {
        sem_wait(&eventMtx);
        lamportClock = max(lamportClock, receivedClock) + 1;
        sem_post(&eventMtx);
    }

    // update number of replies received
    void receiveReply() {
        sem_wait(&eventMtx);
        numRepliesReceived++;
        sem_post(&eventMtx);
    }

    // update number of failed messages received
    void receiveFailed() {
        sem_wait(&eventMtx);
        numFailed++;
        sem_post(&eventMtx);
    }

    // update number of inquire messages received
    void receiveInquire() {
        sem_wait(&eventMtx);
        numInquire++;
        sem_post(&eventMtx);
    }

} pV;

// List of sockets for sending messages to other processes
zmq::socket_t* senders;

// Socket for receiving messages from other processes
zmq::socket_t receiver;

// Context for zmq
zmq::context_t context;

// Initialize the sockets and process variables
void init() {
    // initialize the quorum;
    if(id != n) {
        int q = sqrtl(n);
        // get the grid
        vector<vector<int>> grid(q, vector<int>(q, 0));
        int k = 0;
        for(int i=0; i<q; i++) {
            for(int j=0; j<q; j++) {
                grid[i][j] = k;
                k++;
            }
        }
        // get the row and column of the process
        int row, col;
        for(int i=0; i<q; i++) {
            for(int j=0; j<q; j++) {
                if(grid[i][j] == id) {
                    row = i;
                    col = j;
                    break;
                }
            }
        }
        // get the quorum
        for(int i=0; i<q; i++) {
            if(grid[row][i] != id) {
                pV.quorum.push_back(grid[row][i]);
            }
            if(grid[i][col] != id) {
                pV.quorum.push_back(grid[i][col]);
            }
        }
        pV.quorum.push_back(grid[row][col]);
    }

    // initialize sockets
    senders = new zmq::socket_t[n+1];
    context = zmq::context_t(1);
    receiver = zmq::socket_t(context, ZMQ_PULL);
    receiver.bind(base_endpoint + to_string(5555 + id));
    // 0 to n-1 processes are workers
    // n-th process is parent process, it will send completion message to all processes
    for(int i=0; i<=n; i++) {
        senders[i] = zmq::socket_t(context, ZMQ_PUSH);
        senders[i].connect(base_endpoint + to_string(5555 + i));
    }
}

// Logs the event or sends the message to parent process
void logEvent(string message) {
    zmq::message_t msg(message.size());
    memcpy(msg.data(), message.data(), message.size());
    if(message.substr(0,4)=="exit") senders[n].send(msg, zmq::send_flags::none);
    else {
        // if(message.find("enters") != string::npos || message.find("leaves") != string::npos)
        cout << message << endl;
    }
}

// Function to send message to all processes in the quorum
void sendMsg(string message) {
    for(int i=0; i<pV.quorum.size(); i++) {
        zmq::message_t msg(message.size());
        memcpy(msg.data(), message.data(), message.size());
        senders[pV.quorum[i]].send(msg, zmq::send_flags::none);
        numMessages++;
    }
}

// Function to respond to a specific process
void respond(string message, string receiver) {
    zmq::message_t msg(message.size());
    memcpy(msg.data(), message.data(), message.size());
    senders[stoi(receiver)].send(msg, zmq::send_flags::none);
    numMessages++;
}

// Function to send release message to all the processes in the quorum
void release() {
    for(int i=0; i<pV.quorum.size(); i++) {
        string message = "release " + to_string(id) + " " + to_string(pV.csEntryClock);
        zmq::message_t msg(message.size());
        memcpy(msg.data(), message.data(), message.size());
        senders[pV.quorum[i]].send(msg, zmq::send_flags::none);
        numMessages++;

        // log the event
        message = "p" + to_string(id+1) + " send release to p" + to_string(pV.quorum[i]+1) + " at " + to_string(time(0));
        logEvent(message);
    }
}

void doWork() {
    // random number generators
    default_random_engine generator(time(0) + id);
    exponential_distribution<double> distribution1(1/(2.0*a));
    exponential_distribution<double> distribution2(1/(2.0*b));

    struct timeval beginTot, endTot, beginCS, endCS;

    gettimeofday(&beginTot, 0);
    
    for(int i=0; i<k; i++) {
        // random numbers for time in critical section and out of critical section time
        double outCS = distribution1(generator), inCS = distribution2(generator);
        
        // log the event for local computation
        string message = "p" + to_string(id+1) + " doing local computation at " + to_string(time(0));
        logEvent(message);
        // do some work
        usleep(outCS*1000); // usleep takes time in microseconds. So multiply by 1000 to convert to milliseconds
        
        // request to enter critical section
        // log the event
        message = "p" + to_string(id+1) + " requests to enter CS at " + to_string(time(0)) + " for the " + to_string(i+1) + "th time";
        logEvent(message);
        gettimeofday(&beginCS, 0);
        // set process variables to request to enter critical section
        pV.reqCS();
        // send request to all other processes
        message = "req " + to_string(id) + " " + to_string(pV.csEntryClock);
        sendMsg(message);
        
        // wait for replies
        while(!pV.canEnterCS());
        
        // enter critical section
        // log the event
        message = "p" + to_string(id+1) + " enters CS at " + to_string(time(0));
        logEvent(message);
        // do some work in critical section
        usleep(inCS*1000); // usleep takes time in microseconds. So multiply by 1000 to convert to milliseconds
        
        // exit critical section

        gettimeofday(&endCS, 0);
        totalCS += (endCS.tv_sec - beginCS.tv_sec)*1000000 + endCS.tv_usec - beginCS.tv_usec;

        // log the event
        message = "p" + to_string(id+1) + " leaves CS at " + to_string(time(0)) + " for the " + to_string(i+1) + "th time";
        logEvent(message);
        
        // send release message to all processes in the quorum
        release();
    }
    gettimeofday(&endTot, 0);
    totalRuntime = (endTot.tv_sec - beginTot.tv_sec)*1000000 + endTot.tv_usec - beginTot.tv_usec;

    // send completion message to parent process
    string message = "exit " + to_string(numMessages) + " " + to_string(totalRuntime) + " " + to_string(totalCS);
    logEvent(message);
}

void receive() {
    // receive messages from other processes until exit message is received
    while(1) {
        zmq::message_t msg;
        zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
        string message(static_cast<char*>(msg.data()), msg.size());
        stringstream ss(message);
        string type, sender, clock;
        ss >> type >> sender >> clock;
        
        // received reply message from other process
        if(type == "reply") {
            // Update process variables 
            pV.receiveReply();
            pV.updateClock(stoi(clock));
        }
        
        // received request message from other process
        else if(type == "req") {
            // log the event
            message = "p" + to_string(id+1) + " received p" + to_string(stoi(sender)+1) + "\'s request to enter CS at " + to_string(time(0));
            logEvent(message);

            pV.updateClock(stoi(clock));

            // add the request to queue
            pV.queue.insert(make_pair(stoi(clock), stoi(sender)));
            
            // lock has not been set yet
            if(pV.lockedOn == make_pair(-1,-1)) {
                // set the lock and send reply to the process requesting to enter CS
                pV.lockedOn = make_pair(stoi(clock), stoi(sender));
                respond("reply " + to_string(id) + " " + to_string(pV.lamportClock), sender);

                // log the event
                message = "p" + to_string(id+1) + " reply to p" + to_string(stoi(sender)+1) + "\'s request to enter CS at " + to_string(time(0));
                logEvent(message);
            }
            // lock has been set
            else {
                // if the process has lower priority, send failed message
                if(pV.lockedOn.first < stoi(clock) || (pV.lockedOn.first == stoi(clock) && pV.lockedOn.second < stoi(sender))) {
                    message = "failed " + to_string(id) + " " + to_string(pV.lamportClock);
                    respond(message, sender);

                    // log the event
                    message = "p" + to_string(id+1) + " send failed message to p" + to_string(stoi(sender)+1) + " at " + to_string(time(0));
                    logEvent(message);
                }

                // if the process has higher priority, send inquire message to the process holding the lock
                else {
                    message = "inquire " + to_string(id) + " " + to_string(pV.lamportClock);
                    respond(message, to_string(pV.lockedOn.second));

                    // log the event
                    message = "p" + to_string(id+1) + " send inquire to p" + to_string(pV.lockedOn.second+1) + " at " + to_string(time(0));
                    logEvent(message);
                }
            }
        }

        // received yield message from other process and lock is held by the process sending the yield message
        else if(type == "yield" && pV.lockedOn.second == stoi(sender)) {
            // log the event
            message = "p" + to_string(id+1) + " received yield message from p" + to_string(stoi(sender)+1) + " at " + to_string(time(0));
            logEvent(message);

            pV.updateClock(stoi(clock));

            // set the lock to the process with highest priority in the queue
            pV.lockedOn = *pV.queue.begin();

            // send reply to the process holding the lock
            respond("reply " + to_string(id) + " " + to_string(pV.lamportClock), to_string(pV.lockedOn.second));

            // log the event
            message = "p" + to_string(id+1) + " reply to p" + to_string(pV.lockedOn.second+1) + " at " + to_string(time(0));
            logEvent(message);
        }


        // received failed message from other process
        else if(type == "failed") {
            pV.updateClock(stoi(clock));
            pV.receiveFailed();

            if(pV.numInquire) {
                // send yield message to all processes in the quorum
                string message = "yield " + to_string(id) + " " + to_string(pV.lamportClock);
                sendMsg(message);

                // set the variables
                pV.numRepliesReceived = 0;

                // log the event
                message = "p" + to_string(id+1) + " send yield at " + to_string(time(0));
                logEvent(message);
            }
        }

        // received inquire message from other process
        else if(type == "inquire") {
            pV.updateClock(stoi(clock));
            pV.receiveInquire();

            if(pV.numFailed) {
                // send yield message to all processes in the quorum
                string message = "yield " + to_string(id) + " " + to_string(pV.lamportClock);
                sendMsg(message);

                // set the variables
                pV.numRepliesReceived = 0;

                // log the event
                message = "p" + to_string(id+1) + " send yield at " + to_string(time(0));
                logEvent(message);
            }
        }

        // received release message from other process
        else if(type == "release") {
            pV.updateClock(stoi(clock));

            // remove the process from the queue
            pV.queue.erase(make_pair(stoi(clock), stoi(sender)));

            // if the queue is not empty, send reply to the process with highest priority in the queue
            if(!pV.queue.empty()) {
                pV.lockedOn = *pV.queue.begin();
                respond("reply " + to_string(id) + " " + to_string(pV.lamportClock), to_string(pV.lockedOn.second));

                // log the event
                message = "p" + to_string(id+1) + " reply to p" + to_string(pV.lockedOn.second+1) + "\'s request to enter CS at " + to_string(time(0));
                logEvent(message);
            }
            // if the queue is empty, set the lock to null
            else {
                pV.lockedOn = make_pair(-1,-1);
            }
        }

        // Received exit message from parent process
        else if(type == "exit") {
            break;
        }
    }
}

int main() {
    int i;

    // open the file to read input parameters
    ifstream file;
    file.open("inp-params.txt");

    if(!file) {
        cout << "Error in opening the file" << endl;
        return 0;
    }

    file >> n >> k >> a >> b;

    file.close();

    // check if n is a perfect square
    bool flag = false;
    for(i=sqrtl(n)-1; i<=sqrtl(n)+1; i++) {
        if(i*i == n) {
            flag = true;
            break;
        }
    }
    if(!flag) {
        cout << "Number of processes should be a perfect square" << endl;
        return 0;
    }

    // Clear the log file
    freopen("log-MK.txt", "w", stdout);
    fclose(stdout);

    // create n processes
    for(i=0; i<n; i++) {
        if(fork() == 0) {
            break;
        }
    }

    // set the process id
    id = i;

    // initialize the sockets
    init();

    // parent process
    if(i == n) {
        // run the loop until all processes have sent termination message
        int compCount = 0;
        while(compCount < n) {
            zmq::message_t msg;
            zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
            string message(static_cast<char*>(msg.data()), msg.size());
            // if exit message is received, update the statistics
            if(message.substr(0, 4) == "exit"){
                istringstream ss(message);
                string type, count, totTime, csTime;
                ss >> type >> count >> totTime >> csTime;
                numMessages += stoi(count);
                totalRuntime += stoll(totTime);
                totalCS += stoll(csTime);
                compCount++;
            }
        }

        // send exit message to all processes
        string message = "exit 0 0";
        for(i=0; i<n; i++) {
            zmq::message_t msg(message.size());
            memcpy(msg.data(), message.data(), message.size());
            senders[i].send(msg, zmq::send_flags::none);
        }
        // wait for all processes to complete
        for(i=0; i<n; i++) {
            wait(NULL);
        }

        freopen("MK-Stats.txt", "w", stdout);

        // Calculate the statistics
        double avg = (double)numMessages/(n*k);
        double avgLoad = (double)(n*k)/(totalRuntime/1000000.0);
        double throughput = (double)(n*k)/(totalCS/1000000.0);

        cout << "Average number of messages sent by each process per CS entry: " << avg << endl;
        cout << "Throughput: " << throughput << " CS entries per second" << endl;

        fclose(stdout);
    }
    // child processes
    else {
        freopen("log-MK.txt", "a", stdout);
        // wait for all processes to be ready
        sleep(3);
        // start the threads
        thread t1(doWork), t2(receive);
        // wait for the threads to complete
        t1.join(); t2.join();
        // close the log file
        fclose(stdout);
    }

    // close all sockets
    for(int i=0; i<=n; i++) {
        senders[i].close();
    }
    receiver.close();
}