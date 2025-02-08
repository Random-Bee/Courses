#include <bits/stdc++.h>
using namespace std;
typedef long double ld;
bool check(ld** A, ld* b, ld* x, ld* x1, int n, ld s, ld *m1, ld* w) {
    int i,j;
    ld x2[n],m=0;
    memset(x2,0,sizeof(x2));
    for(i=0; i<n; i++) {
        x1[i] -= x[i];
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            x2[i] += A[i][j]*x1[j];
        }
    }
    for(i=0; i<n; i++) {
        m += x2[i]*x2[i];
    }
    cout << m << endl;
    if(*m1-m>0) *w = 1.9;
    else *w = 0.9;
    *m1 = m;
    if(m<s) return true;
    return false;
}
void gaussSeidel(ld** A, ld* b, ld* x, int n, ld s) {
    int i,j,k=0,index;
    ld x1[n+1],m,w=1,z;
    for(i=0; i<=n; i++) x[i] = 50; //initial guess => x[i] = 50
    x[0] = 0;
    while(true) {
        for(i=0; i<=n; i++) {
            x1[i] = x[i];
        }
        for(i=1; i<=n; i++) {
            z = b[i];
            for(j=1; j<=n; j++) {
                if(i!=j) {
                    z -= A[i][j]*x[j];
                }
            }
            z /= A[i][i];
            x[i] = (1-w)*x[i] + w*z;
            // x[i] = z;
        }
        k++;
        if(check(A,b,x,x1,n,s,&m,&w)) {
            cout << k << endl;
            break;
        }
    }
}
int main() {
    int n,i,j;
    ld **A, *x, *b, s=1e-8,k;
    cin >> n;
    A = new ld*[n*n+1];
    b = new ld[n*n+1];
    x = new ld[n*n+1];
    for (i=0; i<=n*n; i++) {
        A[i] = new long double[n*n+1];
    }
    for(i=0; i<=n*n; i++) {
        for(j=0; j<=n*n; j++) {
            A[i][j] = 0;
        }
    }
    for(i=0; i<=n*n; i++) {
        b[i] = 0;
    }
    k = ((ld)n+1)/2;
    cout << k << endl;
    for(i=1; i<=n; i++) {
        for(j=1; j<=n; j++) {
            if(i < n) A[n*(i-1)+j][n*i+j] = (i-k)*(i-k) + (j-k)*(j-k) + (i-k);
            if(i > 1) A[n*(i-1)+j][n*(i-2)+j] = (i-k)*(i-k) + (j-k)*(j-k) - (i-k);
            A[n*(i-1)+j][n*(i-1)+j] = -4*((i-k)*(i-k) + (j-k)*(j-k));
            if(j < n) A[n*(i-1)+j][n*(i-1) + j+1] = (i-k)*(i-k) + (j-k)*(j-k) + (j-k);
            if(j > 1) A[n*(i-1)+j][n*(i-1) + j-1] = (i-k)*(i-k) + (j-k)*(j-k) - (j-k);
        }
    }
    // for(i=1; i<=n*n; i++) {
    //     for(j=1; j<=n*n; j++) {
    //         cout << A[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    for(i=1; i<=n; i++) {
        for(j=1; j<=n; j++) {
            if(i == n) b[n*(i-1)+j] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (i-k))*50;
            if(i == 1) b[n*(i-1)+j] = -1*((i-k)*(i-k) + (j-k)*(j-k) - (i-k))*50;
            if(j == 1) b[n*(i-1)+j] = -1*((i-k)*(i-k) + (j-k)*(j-k) - (j-k))*50;
            if(j == n) b[n*(i-1)+j] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (j-k))*100;
            if(i == n && j == n) b[n*(i-1)+j] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (i-k))*50 + -1*((i-k)*(i-k) + (j-k)*(j-k) + (j-k))*100;
            if(i == n && j == 1) b[n*(i-1)+j] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (i-k))*50 + -1*((i-k)*(i-k) + (j-k)*(j-k) - (j-k))*50;
            if(j == 1 && i == 1) b[n*(i-1)+j] = -1*((i-k)*(i-k) + (j-k)*(j-k) - (j-k))*50 + -1*((i-k)*(i-k) + (j-k)*(j-k) - (i-k))*50;
            if(j == n && i == 1) b[n*(i-1)+j] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (j-k))*100 + -1*((i-k)*(i-k) + (j-k)*(j-k) - (i-k))*50;
        }
    }
    for(i=1; i<=n; i++) {
        for(j=1; j<=n; j++) {
            cout << b[n*(i-1) + j] << " ";
        }
        cout << endl;
    }
    gaussSeidel(A,b,x,n*n,s);
    for(i=1; i<=n; i++) {
        for(j=1; j<=n; j++) {
            cout << x[n*(i-1) + j] << " ";
        }
        cout << endl;
    }
    for(i=1; i<=n*n; i++) {
        delete A[i];
    }
    delete A;
    delete x;
    delete b;
    return 0;
}