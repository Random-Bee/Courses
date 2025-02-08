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
    // list of processes which will receive the request, list of processes whose request is deferred
    vector<bool> willReceive, deferred;
    // status of process - whether it wants to enter CS, whether it is in CS
    bool wantsCS;
    bool inCS;
    // number of replies expected to enter CS, number of replies received
    int numRepliesExpected;
    int numRepliesReceived;
    // Current lamport clock
    int lamportClock;
    // Clock at which process requested to enter CS
    int csEntryClock;
    sem_t eventMtx;
    
    procVars() {
        lamportClock = 0;
        wantsCS = false; inCS = false;
        sem_init(&eventMtx, 0, 1);
    }

    // request to enter critical section
    // Sets wantsCS to true, increment lamport clock, set csEntryClock to current lamport clock
    void reqCS() {
        sem_wait(&eventMtx);
        wantsCS = true;
        numRepliesExpected = 0;
        numRepliesReceived = 0;
        lamportClock++;
        csEntryClock = lamportClock;
        // number of replies expected is number of processes which will receive the request
        for(int i=0; i<n; i++) {
            if(i!=id && willReceive[i]) {
                numRepliesExpected++;
            }
        }
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

    // enter critical section - set wantsCS to false, set inCS to true
    void enterCS() {
        sem_wait(&eventMtx);
        wantsCS = false;
        inCS = true;
        sem_post(&eventMtx);
    }

    // exit critical section - set inCS to false
    void exitCS() {
        sem_wait(&eventMtx);
        inCS = false;
        sem_post(&eventMtx);
    }

    // update lamport clock
    void updateClock(int receivedClock) {
        sem_wait(&eventMtx);
        lamportClock = max(lamportClock, receivedClock) + 1;
        sem_post(&eventMtx);
    }

    // update willReceive list
    void updateWillReceive(int sender, bool val) {
        sem_wait(&eventMtx);
        willReceive[sender] = val;
        sem_post(&eventMtx);
    }

    // update number of replies received
    void receiveReply() {
        sem_wait(&eventMtx);
        numRepliesReceived++;
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
    // initialize willReceive list
    // For the first request, all processes which have id less than current process will receive the request
    pV.willReceive.resize(n,false);
    for(int i=0; i<id; i++) {
        pV.willReceive[i] = true;
    }
    // initialize deferred list
    pV.deferred.resize(n,false);
    // initialize sockets
    senders = new zmq::socket_t[n+1];
    context = zmq::context_t(1);
    receiver = zmq::socket_t(context, ZMQ_PULL);
    receiver.bind(base_endpoint + to_string(5555 + id));
    // 0 to n-1 processes are workers
    // n-th process is parent process, it will log all the messages and send completion message to all processes
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
        cout << message << endl;
    }
}

// Function to send message to other processes
void sendReq(string message, int receiver) {
    // If receiver = -1, send request to all other processes that will receive the request
    if(receiver == -1) {
        for(int i=0; i<n; i++) {
            if(i != id && pV.willReceive[i]) {
                zmq::message_t msg(message.size());
                memcpy(msg.data(), message.data(), message.size());
                senders[i].send(msg, zmq::send_flags::none);
                numMessages++;
            }
        }
    }

    // Otherwise send request to specific process
    // It happens when the process i receives a request from a process j which is not in willReceive list
    // and process i wants to enter CS. So i sends a request to that process j.
    else {
        pV.updateWillReceive(receiver, true);
        pV.numRepliesExpected++;
        zmq::message_t msg(message.size());
        memcpy(msg.data(), message.data(), message.size());
        senders[receiver].send(msg, zmq::send_flags::none);
        numMessages++;
    }
}

// Function to send replies
void sendReply(string message, string receiver, bool deff) {
    // If deff is true, send reply to all deferred processes
    if(deff) {
        for(int i=0; i<n; i++) {
            if(i != id && pV.deferred[i]) {
                zmq::message_t msg(message.size());
                memcpy(msg.data(), message.data(), message.size());
                senders[i].send(msg, zmq::send_flags::none);
                numMessages++;
                // Update willReceive list and deferred list
                // Since reply is sent to i-th process, it will later receive the request
                pV.updateWillReceive(i, true);
                pV.deferred[i] = false;

                // log the event
                string message = "p" + to_string(id+1) + " reply to p" + to_string(i+1) + "\'s request to enter CS at " + to_string(time(0));
                logEvent(message);
            }
        }
    }

    // If deff is false, send reply to specific process which sent the request
    else {
        zmq::message_t msg(message.size());
        memcpy(msg.data(), message.data(), message.size());
        senders[stoi(receiver)].send(msg, zmq::send_flags::none);
        numMessages++;
        // Update willReceive list
        // Since reply is sent to i-th process, it will later receive the request
        pV.updateWillReceive(stoi(receiver), true);

        // log the event
        string message = "p" + to_string(id+1) + " reply to p" + to_string(stoi(receiver)+1) + "\'s request to enter CS at " + to_string(time(0));
        logEvent(message);
    }
}

void doWork() {
    // random number generators
    default_random_engine generator(time(0) + id);
    exponential_distribution<double> distribution1(1.0/(2.0*a));
    exponential_distribution<double> distribution2(1.0/(2.0*b));

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
        sendReq(message, -1);
        
        // wait for replies
        while(!pV.canEnterCS());
        
        // enter critical section
        pV.enterCS();        
        // log the event
        message = "p" + to_string(id+1) + " enters CS at " + to_string(time(0));
        logEvent(message);
        // do some work in critical section
        usleep(inCS*1000); // usleep takes time in microseconds. So multiply by 1000 to convert to milliseconds
        
        // exit critical section
        pV.exitCS();
        gettimeofday(&endCS, 0);
        totalCS += (endCS.tv_sec - beginCS.tv_sec)*1000000 + endCS.tv_usec - beginCS.tv_usec;
        // log the event
        message = "p" + to_string(id+1) + " leaves CS at " + to_string(time(0)) + " for the " + to_string(i+1) + "th time";
        logEvent(message);
        
        // send reply to all deferred processes
        message = "reply " + to_string(id) + " " + to_string(pV.lamportClock);
        sendReply(message, "", 1);
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
            // Since reply is received from the sender, the sender will not receive the request again
            pV.updateWillReceive(stoi(sender), false);
        }
        
        // received request message from other process
        else if(type == "req") {
            // log the event
            message = "p" + to_string(id+1) + " received p" + to_string(stoi(sender)+1) + "\'s request to enter CS at " + to_string(time(0));
            logEvent(message);
            
            // process is in critical section, defer the sender's request
            if(pV.inCS) {
                pV.deferred[stoi(sender)] = true;
                pV.updateClock(stoi(clock));                
            }
            
            // process wants to enter critical section
            else if(pV.wantsCS) {
                // If process has higher priority, defer the sender's request
                if(pV.csEntryClock < stoi(clock) || (pV.csEntryClock == stoi(clock) && id < stoi(sender))) {
                    pV.deferred[stoi(sender)] = true;
                    pV.updateClock(stoi(clock));
                }

                // Send reply to the sender
                else {
                    pV.updateClock(stoi(clock));
                    // If sender is not in willReceive list, send request to that process
                    if(!pV.willReceive[stoi(sender)]) {
                        message = "req " + to_string(id) + " " + to_string(pV.csEntryClock);
                        sendReq(message, stoi(sender));
                    }
                    // Send reply to the sender
                    message = "reply " + to_string(id) + " " + to_string(pV.lamportClock);
                    sendReply(message, sender, 0);
                }
            }

            // process is not in critical section and does not want to enter critical section
            else {
                // send reply to the sender
                pV.updateClock(stoi(clock));
                pV.updateWillReceive(stoi(sender), true);
                string message = "reply " + to_string(id) + " " + to_string(pV.lamportClock);
                sendReply(message, sender, 0);
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

    // Clear the log file
    freopen("log-RC.txt", "w", stdout);
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

        freopen("RC-Stats.txt", "w", stdout);

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
        freopen("log-RC.txt", "a", stdout);
        // wait for all processes to be ready
        sleep(3);
        // start the threads
        thread t1(doWork), t2(receive);
        t1.join(); t2.join();
        fclose(stdout);
    }

    // close all sockets
    for(int i=0; i<=n; i++) {
        senders[i].close();
    }
    receiver.close();
}