#include "BOCC-CO21BTECH11002.h"
typedef long long ll;

Database db;

// Initializes the transaction
void Transaction::init() {
    startTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();
    readSet.clear();
    writeSet.clear();
    db.addActive(startTime);
}

// Adds the start time of the transaction to the active transactions
void Database::addActive(ll startTime) {
    sem_wait(&activeLock);
    activeTrans.insert(startTime);
    sem_post(&activeLock);
}

// Removes the start time of the transaction from the active transactions
void Database::removeActive(ll startTime) {
    sem_wait(&activeLock);
    activeTrans.erase(activeTrans.find(startTime));
    sem_post(&activeLock);
}

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

// Initializes the database with m data items and initializes the required locks
void initDatabase(ll m) {
    db.arr.resize(m+1);
    for(ll i=0; i<=m; i++) {
        db.arr[i] = getRand(0, INF);
    }
    db.locks.resize(m+1);
    for(ll i=0; i<=m; i++) {
        sem_init(&db.locks[i], 0, 1);
    }
    db.write_list.resize(m+1);
    sem_init(&db.activeLock, 0, 1);
    sem_init(&logLock, 0, 1);
}

// Atomic variable to assign unique transaction ids
atomic<ll> id(0);

// Begins a new transaction
Transaction* begin_trans() {
    Transaction* t = new Transaction();
    ll tid = id.fetch_add(1);
    t->id = tid;
    t->init();
    return t;
}

// Reads the value at index from the database
void read(Transaction* t, ll index, ll& val) {
    sem_wait(&db.locks[index]);
    // If the transaction has written to the data item, it should read the value it wrote
    if(t->writeSet.find(index) != t->writeSet.end()) {
        val = t->writeSet[index];
    }
    // Else, it should read the value from the database
    else {
        val = db.arr[index];
    }
    t->readSet[index] = val;
    sem_post(&db.locks[index]);
}

// Writes the value to the write set of the transaction
void write(Transaction* t, ll index, ll val) {
    // Store the value in the write set of the transaction to be written to the database later
    t->writeSet[index] = val;
}

// Cleans up old transactions from the write_list that will no longer be needed
void collectGarbage(set<ll>& combinedSet) {
    sem_wait(&db.activeLock);
    ll minActiveTime = *db.activeTrans.begin();
    // can remove all transactions with endTime < minActiveTime
    for(auto it: combinedSet) {
        vector<pair<ll, ll>> toRemove;
        for(auto [en, st]: db.write_list[it]) {
            if(en < minActiveTime) {
                toRemove.push_back({en, st});
            }
            else {
                // as the write_list is sorted in increasing order of end time, later elements will also have endTime >= minActiveTime
                break;
            }
        }
        for(auto [en, st]: toRemove) {
            db.write_list[it].erase({en, st});
        }
    }
    sem_post(&db.activeLock);
}

// Tries to commit the transaction
bool tryCommit(Transaction* t) {
    // needs to acquire locks for both read and write sets to ensure correctness of critical section
    set<ll> combinedSet;
    for(auto it: t->readSet) {
        combinedSet.insert(it.first);
    }
    for(auto it: t->writeSet) {
        combinedSet.insert(it.first);
    }
    for(auto it: combinedSet) {
        sem_wait(&db.locks[it]);
    }

    // intersection of readSet of t and writeSet of all committed transactions should be null
    for(auto [ind, val]: t->readSet) {
        ll cnt = 0;
        for(auto [en, st]: db.write_list[ind]) {
            if(en >= t->startTime) { // ended after the start of the current transaction
                cnt++;
            }
        }
        if(cnt > 0) { // intersection is not null, need to abort
            for(auto it: combinedSet) {
                sem_post(&db.locks[it]);
            }
            // remove the transaction from the active transactions. it will be added again when it restarts
            db.removeActive(t->startTime);
            // restart the transaction
            t->init();
            return 0;
        }
    }

    // intersection is null, can commit

    for(auto [ind, val]: t->writeSet) {
        // perform the actual write to the database
        db.arr[ind] = val;

        ll currTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();

        string s = "Transaction id " + to_string(t->id) + " writes to " + to_string(ind) + " a value " + to_string(val) + " in database at time " + to_string(currTime);
        log(s);
    }

    // record the end time of the transaction
    t->endTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();

    // update the write_list of the database for the data items written by the transaction
    for(auto [ind, val]: t->writeSet) {
        db.write_list[ind].insert({t->endTime, t->startTime});
    }

    // remove the transaction from the active transactions
    db.removeActive(t->startTime);
    
    // remove items from write_list for the data items for which we have the lock
    collectGarbage(combinedSet);

    for(auto it: combinedSet) {
        sem_post(&db.locks[it]);
    }

    // Since the transaction has committed, it can be deleted
    delete t;
    
    return 1;
}