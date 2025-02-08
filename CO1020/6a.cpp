#include <bits/stdc++.h>
using namespace std;
typedef long double ld;
ld roundoff(ld value, int prec) {
    ld p = pow(10,prec);
    return round(value*p)/p;
}
bool check(ld** A, ld* b, ld* x, ld* x1, int n, ld s, ld* w) {
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
    // if(m>100) *w = 0.05;
    // else *w = 1;
    if(m<s) return true;
    return false;
}
void gaussSeidel(ld** A, ld* b, ld* x, int n, ld s) {
    int i,j,k=0;
    ld x1[n],z,w=1;
    for(i=0; i<n; i++) x[i] = 1; //initial guess => x[i] = 1
    while(true) {
        for(i=0; i<n; i++) {
            x1[i] = x[i];
        }
        for(i=0; i<n; i++) {
            z = b[i];
            for(j=0; j<n; j++) {
                if(i!=j) {
                    z -= A[i][j]*x[j];
                }
            }
            // x[i] = (1-w)*x[i] + (w*z)/A[i][i];
            x[i] = z/A[i][i];
        }
        k++;
        for(i=0; i<n; i++) {
            cout << x[i] << " ";
        }
        cout << endl;
        w = 1;
        for(i=0; i<n; i++) {
            if(abs(x[i]-x1[i])>s) w = 0;
        }
        // cout << "<> " << w << endl;
        if(check(A,b,x,x1,n,s,&w)) {
        // if(w==1) {
            for(i=0; i<n; i++) {
                x[i] = roundoff(x[i],6);
            }
            cout << k << endl;
            break;
        }
        // cout << "<> " << w << endl;
    }
}
int main() {
    int n,i,j;
    ld **A;
    ld *x, *b;
    ld s;
    s = 1e-7;
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
            // if(A[i][j] == 0) A[i][j]++;
            cin >> A[i][j];
        }
    }
    for(i=0; i<n; i++) {
        b[i] = rand()%10;
        cin >> b[i];
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
    for(i=0; i<n; i++) {
        cout << b[i] << endl;
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