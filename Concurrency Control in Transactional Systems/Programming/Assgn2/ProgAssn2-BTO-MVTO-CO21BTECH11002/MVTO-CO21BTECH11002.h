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
    // map of transaction id to the pair of value it wrote and the set of transaction ids that have read this version (read_list of this version)
    map<ll, pair<ll, set<ll>>> versions;
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

bool tryCommit(Transaction* t);