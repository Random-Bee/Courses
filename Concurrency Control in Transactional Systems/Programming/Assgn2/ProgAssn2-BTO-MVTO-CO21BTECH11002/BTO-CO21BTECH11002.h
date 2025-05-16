#include <bits/stdc++.h>
#include <semaphore.h>
using namespace std;
typedef long long ll;

extern mt19937 rng;
ll getRand(ll a, ll b);

extern FILE* logFile;

void log(string s);

const ll INF = 1e9;

class Item {
    public:
    set<ll> read_ts; // timestamps of transactions that have read this item. This is required to be a set to know the next max_r in case the transaction which had max timestamp has aborted
    ll max_w, val;
    sem_t lock;

    bool canRead(ll timeStamp);

    bool canWrite(ll timeStamp);

    void read(ll timeStamp, ll& loc);

    void write(ll timeStamp, ll loc);
};

// vector of data items representing the database
extern vector<Item> shared;

void initDatabase(ll m);

class Transaction {
    public:
    ll id; // transaction id, also used as timestamp
    // map of data item index to the value read
    map<ll, ll> readSet;
    // map of data item index to the value written
    map<ll, ll> writeSet;
};

Transaction* begin_trans();

bool read(Transaction* t, ll index, ll& locVal);

void write(Transaction* t, ll index, ll locVal);

bool tryCommit(Transaction* t, bool aborted);