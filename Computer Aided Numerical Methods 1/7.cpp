#include <bits/stdc++.h>
using namespace std;
typedef long double ld;
void multiplyMat(ld** A, ld* B, ld* M, int n) {
    int i,j,k;
    ld s=0;
    for (i = 0; i < n; i++) {
        s = 0;
        for (j = 0; j < n; j++) {
            s += A[i][j] * B[j];
        }
        M[i] = s;
    }
}
void conjugateGradient(ld** A, ld* b, ld* x, int n, ld s) {
    int i,j,k=0,index;
    ld *r,*r1,*d,*d1,m,w=1,z,*temp,a,m1,beta;
    r = new ld[n];
    r1 = new ld[n];
    d = new ld[n];
    d1 = new ld[n];
    temp = new ld[n];
    for(i=0; i<n; i++) x[i] = 1;
    multiplyMat(A,x,temp,n);
    for(i=0; i<n; i++) {
        r[i] = b[i]-temp[i];
    }
    for(i=0; i<n; i++) {
        d[i] = r[i];
    }
    m=1;
    while(m>s) {
        cout << m << endl;
        m=0;
        for(i=0; i<n; i++) {
            m += r[i]*r[i];
        }
        multiplyMat(A,d,temp,n);
        a = m;
        m1 = 0;
        for(i=0; i<n; i++) {
            m1 += temp[i]*d[i];
        }
        a /= m1;
        for(i=0; i<n; i++) {
            x[i] = x[i] + d[i]*a;
        }
        for(i=0; i<n; i++) {
            r[i] -= a*temp[i];
        }
        beta = 0;
        for(i=0; i<n; i++) {
            beta -= r[i]*temp[i];
        }
        beta /= m1;
        for(i=0; i<n; i++) {
            d[i] = r[i] + beta*d[i];
        }
    }
    delete r; delete r1; delete d; delete d1; delete temp;
}
int main() {
    int n,i,j;
    ld **A, *x, *b, s = 1e-8;
    cin >> n;
    A = new ld*[n];
    x = new ld[n];
    b = new ld[n];
    for(i=0; i<n; i++) {
        A[i] = new ld[n];
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            // A[i][j] = rand()%10;
            cin >> A[i][j];
        }
    }
    for(i=0; i<n; i++) {
        // b[i] = rand()%10;
        cin >> b[i];
    }
    conjugateGradient(A,b,x,n,s);
    for(i=0; i<n; i++) {
        cout <<  x[i] << " ";
    }
    cout << endl;
    for(i=0; i<n; i++) {
        delete A[i];
    }
    delete A; delete b; delete x;
    return 0;
}