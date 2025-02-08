#include <bits/stdc++.h>
using namespace std;
typedef long double ld;
void multiplyMat(ld** A, ld** B, ld** C, int n) {
    int i,j,k;
    ld s=0;
    ld **M; 
    M = new ld*[n];
    for(i=0; i<n; i++) {
        M[i] = new ld[n];
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0; k < n; k++) {
                s += A[i][k] * B[k][j];
            }
            M[i][j] = s;
            s = 0;
        }
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            C[i][j] = M[i][j];
        }
    }
    for(i=0; i<n; i++) {
        delete M[i];
    }
    delete M;
}
ld norm(ld** A, int n, int j) {
    int i;
    ld s=0;
    for(i=j; i<n; i++) {
        s += A[i][j]*A[i][j];
    }
    return sqrt(s);
}
void QR(ld** A, ld** Q, ld** R, int n) {
    int i,j,k,e;
    ld **h,**h1,nm,*v;
    h = new ld*[n];
    h1 = new ld*[n];
    v = new ld[n];
    for(i=0; i<n; i++) {
        h[i] = new ld[n];
        h1[i] = new ld[n];
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            R[i][j] = A[i][j];
        }
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            if(i==j) h[i][j] = 1;
            else h[i][j] = 0;
        }
    }
    for(j=0; j<n; j++) {
        for(i=0; i<n; i++) {
            for(k=0; k<n; k++) {
                if(i==k) h1[i][k] = 1;
                else h1[i][k] = 0;
            }
        }
        nm = norm(R,n,j);
        for(i=j; i<n; i++) {
            if(i==j) e = 1;
            else e = 0;
            v[i] = R[i][j] + (R[i][j]/abs(R[i][j]))*norm(R,n,j)*e;
        }
        nm = 0;
        for(i=j; i<n; i++) {
            nm += v[i]*v[i];
        }
        for(i=j; i<n; i++) {
            for(k=j; k<n; k++) {
                if(i==k) e = 1;
                else e=0;
                h1[i][k] = e - (v[i]*v[k])*2/nm;
            }
        }
        multiplyMat(h1,R,R,n);
        multiplyMat(h1,h,h,n);
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            Q[i][j] = h[j][i];
        }
    }
    for(i=0; i<n; i++) {
        delete h[i]; delete h1[i];
    }
    delete h; delete h1; delete v;
}
bool check(ld** A, int n) {
    int i,j;
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            if(i != j && abs(A[i][j]) > 1e-10) return false;
        }
    }
    return true;
}
void getEigen(ld** A, ld** Q, ld** R, ld** E, int n) {
    int i,j,k;
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            if(i==j) E[i][j] = 1;
            else E[i][j] = 0; 
        }
    }
    k=10;
    while (k>0) {
        k--;
        QR(A,Q,R,n);
        multiplyMat(R,Q,A,n);
        multiplyMat(E,Q,E,n);
        if(check(A,n)) {
            break;
        }
    }
}
int main() {
    int i,j,n;
    ld **A, **Q, **R, **E;
    cin >> n;
    A = new ld*[n];
    Q = new ld*[n];
    R = new ld*[n];
    E = new ld*[n];
    for(i=0; i<n; i++) {
        A[i] = new ld[n];
        Q[i] = new ld[n];
        R[i] = new ld[n];
        E[i] = new ld[n];
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            cin >> A[i][j];
        }
    }
    getEigen(A,Q,R,E,n);
    cout << "Eigen Values:" << endl;
    for(i=0; i<n; i++) {
        cout << A[i][i] << " ";
    }
    cout << endl;
    cout << "Eigen vectors:" << endl;
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            cout << E[i][j] << "  ";
        }
        cout << endl;
    }
    for(i=0; i<n; i++) {
        delete A[i]; delete Q[i]; delete R[i]; delete E[i];
    }
    delete A; delete Q; delete R; delete E;
    return 0;
}