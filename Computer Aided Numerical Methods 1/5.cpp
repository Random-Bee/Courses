#include <bits/stdc++.h>
using namespace std;
typedef long double ld;
void bs(ld **u, ld* b, ld *x, int n) {
    int i,j;
    ld x1[n],sum;
    x1[n-1] = b[n-1]/u[n-1][n-1];
    for(i=n-2; i>=0; i--) {
        sum = 0;
        for(j=i+1; j<n; j++) {
            sum += u[i][j]*x1[j];
        }
        x1[i] = (b[i] - sum)/u[i][i];
    }
    for(i=0; i<n; i++) {
        x[i] = x1[i];
    }
}
void fs(ld **l, ld* b, ld *x, int n) {
    int i,j;
    ld x1[n],sum;
    x1[0] = b[0]/l[0][0];
    for(i=1; i<n; i++) {
        sum = 0;
        for(j=0; j<i; j++) {
            sum += l[i][j]*x1[j];
        }
        // cout << sum << " " << b[i] << " " << l[i][i] << endl;
        x1[i] = (b[i] - sum)/l[i][i];
    }
    // for(i=0; i<n; i++) {
    //     cout << x1[i] << " ";
    // }
    // cout << endl;
    for(i=0; i<n; i++) {
        x[i] = x1[i];
    }
}
void lu(ld **A, ld *b, ld *x, int n) {
    int i=0,j=0,k=0;
    ld **u, **l;
    u = new ld*[n];
    l = new ld*[n];
    for(i=0; i<n; i++) {
        u[i] = new ld[n];
        l[i] = new ld[n];
    }
    for(i=0; i<n; i++) {
        l[i][i] = 1;
    }
    for(j=0; j<n; j++) {
        for(i=0; i<=j; i++) {
            if(i != j) l[i][j] = 0;
            u[i][j] = A[i][j];
            for(k=0; k<i; k++) {
                u[i][j] -= l[i][k]*u[k][j];
            }
        }
        for(i=j+1; i<n; i++) {
            u[i][j] = 0;
            l[i][j] = A[i][j];
            for(k=0; k<j; k++) {
                l[i][j] -= l[i][k]*u[k][j];
            }
            l[i][j] /= u[j][j];
        }
    }
    cout << "l" << endl;
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            cout << l[i][j] << " ";
        }
        cout << endl;
    }
    cout << "u" << endl;
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            cout << u[i][j] << " ";
        }
        cout << endl;
    }
    fs(l,b,x,n);
    // for(i=0; i<n; i++) {
    //     cout << x[i] << " ";
    // }
    bs(u,x,x,n);
    for(i=0; i<n; i++) {
        cout << x[i] << " ";
    }
    cout << endl;
    for(i=0; i<n; i++) {
        delete l[i];
        delete u[i];
    }
    delete u; delete l;
}
int main() {
    int n,i,j;
    ld **A, *x, *b;
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
    lu(A,b,x,n);
    // for(i=0; i<n; i++) {
    //     cout << x[i] << " ";
    // }
    // cout << endl;
    for(i=0; i<n; i++) {
        delete A[i];
    }
    delete A; delete b; delete x;
    return 0;
}