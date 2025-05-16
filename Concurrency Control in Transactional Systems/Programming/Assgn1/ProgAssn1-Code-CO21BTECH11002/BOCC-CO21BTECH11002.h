#include <bits/stdc++.h>
#include <semaphore.h>
using namespace std;
typedef long long ll;

extern mt19937 rng;
ll getRand(ll a, ll b);

extern FILE* logFile;

void log(string s);

const ll INF = 1e9;

class Database {
    public:
    // data items and locks
    vector<ll> arr;
    vector<sem_t> locks;
    // <end time, start time> so to keep sorted in increasing order of end time
    // set is fine since no two transactions end at the same time so no two pairs will be equal
    vector<set<pair<ll,ll>>> write_list;
    // start time of all active transactions
    // multiset since multiple transactions can start at the same time
    multiset<ll> activeTrans;
    sem_t activeLock;

    void addActive(ll tid);
    void removeActive(ll tid);
};

void initDatabase(ll n);

class Transaction {
    public:
    ll id;
    // start and end times of the transaction
    ll startTime, endTime;
    // map of data item index to value read
    map<ll, ll> readSet;
    // map of data item index to value written
    map<ll, ll> writeSet;
    void init();
};

Transaction* begin_trans();

void read(Transaction* t, ll index, ll& val);

void write(Transaction* t, ll index, ll val);

bool tryCommit(Transaction* t);