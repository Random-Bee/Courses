/*
Name :- Aayush Kumar
Roll No. :- CO21BTECH11002
Assignment 1
*/

#include<bits/stdc++.h>
using namespace std;
int main() {
    long long a, b, c, i, j, s = 0, k = 0;
    vector<vector <long long>> m,p,r;
    vector<long long> temp;
    cin >> a >> b >> c;
    srand(0);
    for(i=0; i<a; i++) {
        m.push_back(temp);
        for(j=0; j<b; j++) {
            m[i].push_back(rand()%9);
        }
    }
    for(i=0; i<b; i++) {
        p.push_back(temp);
        for(j=0; j<c; j++) {
            p[i].push_back(rand()%9);
        }
    }
    for(i=0; i<a; i++) {
        r.push_back(temp);
        for(j=0; j<c; j++) {
            r[i].push_back(0);
        }
    }
    for (i = 0; i < a; i++) {
        for (j = 0; j < c; j++) {
            for (k = 0; k < b; k++) {
                s += m[i][k] * p[k][j];
            }
            r[i][j] = s;
            s = 0;
        }
    }
    for(i=0; i<a; i++) {
        for(j=0; j<c; j++) {
            cout << m[i][j] << " ";
        }
        cout << endl;;
    }
    cout << endl;
    for(i=0; i<a; i++) {
        for(j=0; j<c; j++) {
            cout << p[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    for(i=0; i<a; i++) {
        for(j=0; j<c; j++) {
            cout << r[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}