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
            v[i] = R[i][j] + (R[i][i]/abs(R[i][i]))*nm*e;
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
        for(j=0; j<i; j++) {
            if(abs(A[i][j]) > 1e-10) return false;
        }
    }
    return true;
}
int getEigenQR(ld** A, ld** Q, ld** R, ld** E, int n) {
    int i,j,k,c=0;
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            if(i==j) E[i][j] = 1;
            else E[i][j] = 0; 
        }
    }
    while (true) {
        c++;
        QR(A,Q,R,n);
        multiplyMat(R,Q,A,n);
        multiplyMat(E,Q,E,n);
        if(check(A,n)) {
            break;
        }
    }
    return c;
}
void hess(ld** A, int n) {
    int i,j,k,e;
    ld **h1,nm,*v;
    h1 = new ld*[n];
    v = new ld[n];
    for(i=0; i<n; i++) {
        h1[i] = new ld[n];
    }
    for(j=0; j<n-2; j++) {
        for(i=0; i<n; i++) {
            for(k=0; k<n; k++) {
                if(i==k) h1[i][k] = 1;
                else h1[i][k] = 0;
            }
        }
        nm = norm(A,n,j);
        for(i=j+1; i<n; i++) {
            if(i==j+1) e = 1;
            else e = 0;
            v[i] = norm(A,n,j)*e - A[i][j];
        }
        nm = 0;
        for(i=j+1; i<n; i++) {
            nm += v[i]*v[i];
        }
        for(i=j+1; i<n; i++) {
            for(k=j+1; k<n; k++) {
                if(i==k) e = 1;
                else e=0;
                h1[i][k] = e - (v[i]*v[k])*2/nm;
            }
        }
        multiplyMat(h1,A,A,n);
        multiplyMat(A,h1,A,n);
    }
    for(i=0; i<n; i++) {
        delete h1[i];
    }
    delete h1; delete v;
}
int getEigenHess(ld** A, ld** Q, ld** R, int n) {
    int i,j,k,c=0;
    ld sg;
    i = n-1; j = n-1;
    for(j=n-1; j>0; j--) {
        while(true) {
            c++;
            sg = A[j][j];
            while(abs(sg*sg - (A[j][j] + A[j-1][j-1])*sg + A[j-1][j-1]*A[j][j] - A[j][j-1]*A[j-1][j]) > 1e-8) {
                sg = sg - (sg*sg - (A[j][j] + A[j-1][j-1])*sg + A[j-1][j-1]*A[j][j] - A[j][j-1]*A[j-1][j])/(2*sg - (A[j][j] + A[j-1][j-1]));
            }
            if(abs(sg - A[j][j]) > abs(sg - A[j-1][j-1])) sg = A[j][j] + A[j-1][j-1] - sg;
            for(i=0; i<=j; i++) {
                A[i][i] -= sg;
            }
            QR(A,Q,R,n);
            multiplyMat(R,Q,A,n);
            for(i=0; i<=j; i++) {
                A[i][i] += sg;
            }
            if(abs(A[j][j-1])<1e-4) {
                for(i=j-1; i>=0; i--) {
                    A[i][j] = 0;
                    A[j][i] = 0;
                }
                break;
            }
        }
    }
    return c;
}
int main() {
    int i,j,n,c;
    ld **A1, **A2, **Q, **R, **E;
    clock_t start,end;
    double time;
    cout << endl << "Enter the dimension of matrix : ";
    cin >> n;
    A1 = new ld*[n];
    A2 = new ld*[n];
    Q = new ld*[n];
    R = new ld*[n];
    E = new ld*[n];
    for(i=0; i<n; i++) {
        A1[i] = new ld[n];
        A2[i] = new ld[n];
        Q[i] = new ld[n];
        R[i] = new ld[n];
        E[i] = new ld[n];
    }
    cout << "Enter the elements of matrix:" << endl;
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            cin >> A1[i][j];
            A2[i][j] = A1[i][j];
        }
    }

    start = clock();
    c = getEigenQR(A1,Q,R,E,n);
    end = clock();
    time = double(end - start) / double(CLOCKS_PER_SEC);
    cout << endl << "Eigen Values using QR Factorization:" << endl;
    for(i=0; i<n; i++) {
        cout << A1[i][i] << endl;
    }
    cout << endl;
    cout << "Number of iterations - " << c << endl << endl;
    cout << "Time Taken - " << time << endl << endl;

    start = clock();
    hess(A2,n);
    c = getEigenHess(A2,Q,R,n);
    end = clock();
    time = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "Eigen Values by first calculating Hessenberg Matrix:" << endl;
    for(i=0; i<n; i++) {
        cout << A2[i][i] << endl;
    }
    cout << endl;
    cout << "Number of iterations - " << c << endl << endl;
    cout << "Time Taken - " << time << endl << endl;

    for(i=0; i<n; i++) {
        delete A1[i]; delete A2[i]; delete Q[i]; delete R[i]; delete E[i];
    }
    delete A1; delete A2; delete Q; delete R; delete E;
    return 0;
}