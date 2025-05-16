#include "BTO-CO21BTECH11002.h"
typedef long long ll;


// General functions

// Random number generator
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

// Returns a random number in the range [a, b]
ll getRand(ll a, ll b) {
    return uniform_int_distribution<ll>(a, b)(rng);
}

FILE* logFile;

sem_t logLock;

// Logs the string s to the log file
void log(string s) {
    sem_wait(&logLock);
    fprintf(logFile, "%s\n", s.c_str());
    sem_post(&logLock);
}


// Item class functions

// checks if the transaction can read the value of the item
bool Item::canRead(ll timeStamp) {
    bool can_read = (timeStamp >= max_w);
    return can_read;
}

// checks if the transaction can write to the item
bool Item::canWrite(ll timeStamp) {
    // rbegin() returns the last element in the set, which is the maximum timestamp
    bool can_write = (timeStamp >= max(*read_ts.rbegin(), max_w));
    return can_write;
}

// reads the value of the item and adds the transaction's timestamp to the read_ts set
void Item::read(ll timeStamp, ll& loc) {
    loc = val;
    read_ts.insert(timeStamp);
}

// writes the value to the item and updates the maximum write timestamp
void Item::write(ll timeStamp, ll loc) {
    val = loc;
    max_w = timeStamp;
}

vector<Item> shared;

// Initializes the database with m data items and initializes the required locks
void initDatabase(ll m) {
    shared.resize(m+1);
    for(ll i=0; i<=m; i++) {
        shared[i].val = getRand(0, INF);
        
        sem_init(&shared[i].lock, 0, 1);

        // 0 is the initial timestamp
        shared[i].max_w = 0;
        shared[i].read_ts.insert(0);
    }
    sem_init(&logLock, 0, 1);
}

// Atomic variable to assign unique transaction ids
// The transaction ids are also used as timestamps to ensure that the timestamps are unique
atomic<ll> id(1);

// Begins a new transaction
Transaction* begin_trans() {
    Transaction* t = new Transaction();
    ll tid = id.fetch_add(1);
    t->id = tid;
    return t;
}

// Tries to read the value at index from the database and returns whether the transaction was able to read the value
bool read(Transaction* t, ll index, ll& locVal) {
    bool can_read = false;

    sem_wait(&shared[index].lock);
    
    // If the transaction has written to the data item, it should read the value it wrote
    if(t->writeSet.find(index) != t->writeSet.end()) {
        locVal = t->writeSet[index];
        can_read = true;
    }
    // Else, it should read the value from the database
    else {
        // Check if the transaction can read the value
        can_read = shared[index].canRead(t->id);
        // If it can read, read the value
        if(can_read) {
            shared[index].read(t->id, locVal);
        }
    }
    
    // If the transaction has read the value, it should be added to the read set
    if(can_read) {
        t->readSet[index] = locVal;
    }
    
    sem_post(&shared[index].lock);

    return can_read;
}

// Writes the value to the write set of the transaction
void write(Transaction* t, ll index, ll val) {
    // Store the value in the write set of the transaction to be written to the database later
    t->writeSet[index] = val;
}

// Tries to commit the transaction
bool tryCommit(Transaction* t, bool aborted) {
    // whether the transaction commits or aborts, it needs to acquire locks for both read and write sets
    // since it needs to modify the read_ts set and write to the database. so acquire all locks first
    set<ll> combinedSet;
    for(auto [ind, val]: t->readSet) {
        combinedSet.insert(ind);
    }
    for(auto [ind, val]: t->writeSet) {
        combinedSet.insert(ind);
    }
    for(auto ind: combinedSet) {
        sem_wait(&shared[ind].lock);
    }

    // If aborted is true, then the transaction has aborted and it should not commit
    if(aborted) {
        // it should remove itself from the read_ts set of all the items it has read
        for(auto [ind, val]: t->readSet) {
            shared[ind].read_ts.erase(t->id);
        }

        // release all locks
        for(auto ind: combinedSet) {
            sem_post(&shared[ind].lock);
        }

        delete t; // free the memory allocated for the transaction

        return 0; // transaction aborted
    }

    bool can_commit = true;

    // now the transaction will try to actually write to the database
    // so first check if it can write to all the items in the write set
    // if any of the items in the write set cannot be written to, then the transaction should abort
    for(auto [ind, val]: t->writeSet) {
        can_commit &= shared[ind].canWrite(t->id);
    }

    // if it can not commit, then it should abort and remove itself from the read_ts set of all the items it has read

    if(!can_commit) {
        for(auto [ind, val]: t->readSet) {
            shared[ind].read_ts.erase(t->id);
        }

        // release all locks
        for(auto ind: combinedSet) {
            sem_post(&shared[ind].lock);
        }

        delete t; // free the memory allocated for the transaction

        return 0; // transaction aborted
    }

    // now the transaction can commit, so it should write to the database

    for(auto [ind, val]: t->writeSet) {

        shared[ind].write(t->id, val);

        ll currTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();

        string s = "Transaction id " + to_string(t->id) + " writes to " + to_string(ind) + " a value " + to_string(val) + " in database at time " + to_string(currTime);
        log(s);
    }

    // now that the transaction has committed, any future transaction when reading from read_ts will read a timestamps that is greater than or equal to the current transaction's timestamp
    // so we can remove all timestamps that are less than the current transaction's timestamp
    // this is done to ensure that the read_ts set does not grow indefinitely
    for(auto [ind, val]: t->readSet) {
        vector<ll> toErase;

        for(auto it : shared[ind].read_ts) {
            if(it < t->id) {
                toErase.push_back(it);
            }
            else {
                // timestamps are sorted in increasing order, so we can break the loop
                break;
            }
        }

        for(auto it : toErase) {
            shared[ind].read_ts.erase(it);
        }
        // to ensure that the current transaction's timestamp is also in the read_ts set
        shared[ind].read_ts.insert(t->id);
    }

    // release all locks
    for(auto ind: combinedSet) {
        sem_post(&shared[ind].lock);
    }

    delete t; // free the memory allocated for the transaction

    return 1; // transaction committed
}