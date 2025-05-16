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
    // ids of active transactions that have read the data item
    vector<set<ll>> read_list;
};

void initDatabase(ll n);

class Transaction {
    public:
    ll id;
    ll startTime;
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