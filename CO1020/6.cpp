#include <bits/stdc++.h>
using namespace std;
typedef long double ld;
ld roundoff(ld value, int prec) {
    ld p = pow(10,prec);
    return round(value*p)/p;
}

int findmax(ld** A, int i, int j, int n) {
    ld maxval = A[i][j];
    int index = i, p;
    for (p=0; p<n; p++) {
        if (A[j][p] > maxval) {
            maxval = A[j][p];
            index = p;
        }
    }
    return index;
}

bool check(ld** A, ld* b, ld* x, ld* x1, int n, ld s) {
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
    // cout << m << endl;
    if(m<s) return true;
    return false;
}

void gaussSeidel(ld** A, ld* b, ld* x, int n, ld s) {
    int i,j,k=0,index;
    ld x1[n];
    for(i=0; i<n; i++) x[i] = 1; //initial guess => x[i] = 1
    // for(i=0; i<n; i++) {
    //     // making matrix diagonal dominant
    //     index = findmax(A,i,i,n);
    //     A[i][i] = A[i][index] + n;
    // }
    while(true) {
        for(i=0; i<n; i++) {
            x1[i] = x[i];
        }
        for(i=0; i<n; i++) {
            x[i] = b[i];
            for(j=0; j<n; j++) {
                if(i!=j) {
                    x[i] -= A[i][j]*x[j];
                }
            }
            x[i] /= A[i][i];
        }
        k++;
        if(check(A,b,x,x1,n,s)) {
            for(i=0; i<n; i++) {
                x[i] = roundoff(x[i],4);
            }
            cout << k << endl;
            break;
        }
    }
}
int main() {
    int n,i,j;
    ld **A;
    ld *x, *b;
    ld s;
    s = 1e-8;
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
    gaussSeidel(A,b,x,n,s);
    for(i=0; i<n; i++) {
        cout << x[i] << " ";
    }
    cout << endl;
    for(i=0; i<n; i++) {
        delete A[i];
    }
    delete A; delete b; delete x;
    return 0;
}