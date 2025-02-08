#include <bits/stdc++.h>
using namespace std;
void TDMA(long double **A, long double **B, long double *x, long double *P, long double *Q, int n, int T1, int T2) {
    int i;
    P[0] = -A[0][1]/A[0][0];
    Q[0] = B[0][0]/A[0][0];
    for(i=1; i<n-1; i++) {
        P[i] = (-1*A[i][i+1])/(A[i][i] + A[i][i-1]*P[i-1]);
    }
    for(i=1; i<n; i++) {
        Q[i] = (B[i][0] - A[i][i-1]*Q[i-1])/(A[i][i] + A[i][i-1]*P[i-1]);
    }
    x[n-1] = Q[n-1];
    for(i=n-2; i>=0; i--) {
        x[i] = P[i]*x[i+1] + Q[i];
    }
    cout << T1 << " ";
    for(i=0; i<n; i++) {
        cout << x[i] << " ";
    }
    cout << T2 << endl;
}
int main() {
    int n,i,j,k=0,T1,T2;
    long double **A, *x, **B;
    long double *P, *Q;
    cout << "Number of divisions : ";
    cin >> n;
    cout << "Initial Temperature : ";
    cin >> T1;
    cout << "Final Temperature : ";
    cin >> T2;
    A = new long double*[n];
    P = new long double[n];
    Q = new long double[n];
    x = new long double[n];
    for (i=0; i<n; i++) {
        A[i] = new long double[n];
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            A[i][j] = 0;
        }
    }
    B = new long double*[n];
    for (i=0; i<n; i++) {
        B[i] = new long double[1];
    }
    for(i=0; i<n; i++) {
        if(i-1 >= 0) A[i][i-1] = 1;
        A[i][i] = -2;
        if(i+1<n) A[i][i+1] = 1;
    }
    for(i=0; i<n; i++) {
        for(j=0; j<1; j++) {
            B[i][j] = 0;
        }
    }
    B[0][0] = -1*T1;
    B[n-1][0] = -1*T2;
    TDMA(A,B,x,P,Q,n,T1,T2);
    for(i=0; i<n; i++) {
        delete A[i];
        delete B[i];
    }
    delete A;
    delete x;
    delete B;
    delete P;
    delete Q;
    return 0;
}