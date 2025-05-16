#include "FOCC-CTA-CO21BTECH11002.h"
typedef long long ll;

// Initializes the transaction
void Transaction::init() {
    startTime = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch()).count();
    readSet.clear();
    writeSet.clear();
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

Database db;

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
    db.read_list.resize(m+1);
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

// Reads the value at index from the database into val
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
    // add the transaction to the read list of the data item
    db.read_list[index].insert(t->id);
    sem_post(&db.locks[index]);
}

// Writes the value to the write set of the transaction
void write(Transaction* t, ll index, ll val) {
    // If the transaction reads the value at index again, it should read the value it wrote
    t->writeSet[index] = val;
}

// Tries to commit the transaction
bool tryCommit(Transaction* t) {
    // whether the transaction commits or aborts, it needs to acquire locks for both read and write sets
    // since it needs to modify the read list and write to the database. so acquire all locks first
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

    // intersection of writeSet of t and readSet of all active transactions should be null
    for(auto [ind, val]: t->writeSet) {
        
        db.read_list[ind].erase(t->id); // remove the transaction from the read list of the data item so it doesn't count its own read
        // no need to add it again since whether the transaction commits or aborts, it will be removed from the read list anyway
        // if the transaction had not read the data item, it won't be in the read list. calling erase on a key not in the set is a no-op and doesn't throw an error

        if(db.read_list[ind].size() > 0) { // intersection is not null, need to abort
            // remove itself from the read list
            for(auto it: t->readSet) {
                db.read_list[it.first].erase(t->id);
            }

            for(auto it: combinedSet) {
                sem_post(&db.locks[it]);
            }
            
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

    // remove itself from the read list of the data items it read
    for(auto [ind, val]: t->readSet) {
        db.read_list[ind].erase(t->id);
    }
    
    for(auto it: combinedSet) {
        sem_post(&db.locks[it]);
    }

    // since the transaction has committed, it can be deleted
    delete t;
    
    return 1;
}