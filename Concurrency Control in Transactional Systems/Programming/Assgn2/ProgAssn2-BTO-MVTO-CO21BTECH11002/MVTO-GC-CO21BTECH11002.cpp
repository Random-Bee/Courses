#include "MVTO-GC-CO21BTECH11002.h"
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
    // a version with timestamp smaller than or equal to the transaction's timestamp should exist
    // so current transaction's timestamp should not be the smallest in the versions map
    // In MVTO with unbounded number of versions, t0 (with timestamp 0) will always be there in the versions map
    // so it will always return true
    return timeStamp >= versions.begin()->first;
}

// checks if the transaction can write to the item
bool Item::canWrite(ll timeStamp) {
    // two conditions:
    // 1. same as canRead
    // 2. the transaction's timestamp should not come between timestamps of ti and tj, where ti has written to the item and tj has read the item version created by ti
    // condition 1:
    bool cond1 = (timeStamp >= versions.begin()->first);
    if(!cond1) {
        return false;
    }
    // condition 2:
    // if the transaction's timestamp comes between timestamps of ti and tj, then ts_ti < timeStamp < ts_tj
    auto iter = versions.lower_bound(timeStamp);
    iter--; // iter now points to the version with the largest timestamp less than the current transaction's timestamp
    ll max_ts = *((iter->second.second).rbegin()); // the maximum timestamp of the transactions that have read this version

    // iter->first is the timestamp of the transaction that wrote this version
    // max_ts is the maximum timestamp of the transactions that have read this version
    bool cond2 = !(iter->first < timeStamp && timeStamp < max_ts);

    return cond2;
}

// reads the value of the item and adds the transaction's timestamp to the read_ts set
void Item::read(ll timeStamp, ll& loc) {
    // get the version with the largest timestamp less than or equal to the transaction's timestamp
    auto iter = versions.lower_bound(timeStamp);
    iter--; // iter now points to the version with the largest timestamp less than or equal to the transaction's timestamp

    loc = iter->second.first; // the value of the item is the first element of the pair

    // add the transaction's timestamp to the read_list of the version
    iter->second.second.insert(timeStamp);
}

// writes the value to the item and updates the maximum write timestamp
void Item::write(ll timeStamp, ll loc) {
    // create a new version with the current transaction's timestamp and the value to be written
    versions[timeStamp] = {loc, {}};
}

vector<Item> shared;

// set of timestamps of active transactions
set<ll> activeTrans;
sem_t activeLock;

// Initializes the database with m data items and initializes the required locks
void initDatabase(ll m) {
    shared.resize(m+1);
    for(ll i=0; i<=m; i++) {
        // initialize the value of the item, assuming t0 (with timestamp 0) as the initialization transaction
        shared[i].versions[0] = {getRand(0, INF), {}};

        sem_init(&shared[i].lock, 0, 1);
    }
    sem_init(&logLock, 0, 1);
    sem_init(&activeLock, 0, 1);
}

// Adds the timestamp of the transaction to the active transactions
void addActive(ll timeStamp) {
    sem_wait(&activeLock);
    activeTrans.insert(timeStamp);
    sem_post(&activeLock);
}

// Removes the timestamp of the transaction from the active transactions
void removeActive(ll timeStamp) {
    sem_wait(&activeLock);
    activeTrans.erase(timeStamp);
    sem_post(&activeLock);
}

// Atomic variable to assign unique transaction ids
// The transaction ids are also used as timestamps to ensure that the timestamps are unique
atomic<ll> id(1);

// Begins a new transaction
Transaction* begin_trans() {
    Transaction* t = new Transaction();
    ll tid = id.fetch_add(1);
    t->id = tid;
    // add the transaction's timestamp to the activeTrans set
    addActive(tid);
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

// Function to remove the versions created by very old transactions that are no longer required
// We do this for all the items for which we have the lock
void collectGarbage(set<ll>& combinedSet) {
    sem_wait(&activeLock);
    // get the minimum timestamp of the active transactions
    if(activeTrans.empty()) {
        sem_post(&activeLock);
        return;
    }
    ll minActive = *activeTrans.begin();
    sem_post(&activeLock);

    // For writing to the database, transaction ti needs to check whether it comes between timestamps of tj and tk (condition 2)
    // For a given item, let's say that ts is the biggest timestamp of a transaction that has written to the item and ts < minActive
    // Then ts will be required by minActive to check for condition 2
    // All smaller timestamps can be removed

    for(auto ind: combinedSet) {
        auto iter = shared[ind].versions.lower_bound(minActive);
        iter--; // iter now points to the version with the largest timestamp less than minActive
        // we need to leave iter and remove all the versions before iter
        vector<ll> toRemove;
        for(auto it = shared[ind].versions.begin(); it != iter; it++) {
            toRemove.push_back(it->first);
        }
        for(auto it: toRemove) {
            shared[ind].versions.erase(it);
        }
    }
}

// Tries to commit the transaction
bool tryCommit(Transaction* t) {
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

    bool can_commit = true;

    // now the transaction will try to actually write to the database
    // so first check if it can write to all the items in the write set
    // if any of the items in the write set cannot be written to, then the transaction should abort
    for(auto [ind, val]: t->writeSet) {
        can_commit &= shared[ind].canWrite(t->id);
    }

    // if it can not commit, then it should abort and remove itself from the read_list of version of all the items it has read

    if(!can_commit) {
        for(auto [ind, val]: t->readSet) {
            auto iter = shared[ind].versions.lower_bound(t->id);
            iter--; // iter now points to the version that the transaction has read
            // remove the transaction's timestamp from the read_list of the version
            iter->second.second.erase(t->id);
        }

        // release all locks
        for(auto ind: combinedSet) {
            sem_post(&shared[ind].lock);
        }

        removeActive(t->id); // remove the transaction's timestamp from the active transactions

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

    removeActive(t->id); // remove the transaction's timestamp from the active transactions

    // collect garbage for all the items for which we have the lock
    collectGarbage(combinedSet);

    // release all locks
    for(auto ind: combinedSet) {
        sem_post(&shared[ind].lock);
    }

    delete t; // free the memory allocated for the transaction

    return 1; // transaction committed
}