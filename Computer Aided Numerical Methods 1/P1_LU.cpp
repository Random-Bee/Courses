#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
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
        x1[i] = (b[i] - sum)/l[i][i];
    }
    for(i=0; i<n; i++) {
        x[i] = x1[i];
    }
}
void luDecomposition(ld **A, ld *b, ld *x, int n) {
    int i=0,j=0,k=0;
    ld **u, **l, sum;
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
        cout << j << endl;
        for(i=0; i<=j; i++) {
            if(i != j) l[i][j] = 0;
            sum = A[i][j];
            for(k=0; k<i; k++) {
                sum -= l[i][k]*u[k][j];
            }
            u[i][j] = sum;
        }
        for(i=j+1; i<n; i++) {
            u[i][j] = 0;
            sum = A[i][j];
            for(k=0; k<j; k++) {
                sum -= l[i][k]*u[k][j];
            }
            l[i][j] = sum/u[j][j];
        }
    }
    fs(l,b,x,n); bs(u,x,x,n);
    for(i=0; i<n; i++) {
        delete l[i];
        delete u[i];
    }
    delete u; delete l;
}
int main() {
    int n,i,j;
    ld **A, *x, *b, s=1e-8,k,T1,T2,c,dx;
    cout << "Enter number of divisions : ";
    cin >> n;
    cout << "Enter length of each division : ";
    cin >> dx;
    cout << "Enter value of constant in gamma : ";
    cin >> c;
    cout << "T1 : ";
    cin >> T1;
    cout << "T2 : ";
    cin >> T2;
    // gamma = x^2 + y^2 + c
    A = new ld*[n*n];
    b = new ld[n*n];
    x = new ld[n*n];
    for (i=0; i<n*n; i++) {
        A[i] = new long double[n*n];
    }
    for(i=0; i<n*n; i++) {
        for(j=0; j<n*n; j++) {
            A[i][j] = 0;
        }
    }
    for(i=0; i<n*n; i++) {
        b[i] = 0;
    }
    k = ((ld)n+1)/2;
    for(i=1; i<=n; i++) {
        for(j=1; j<=n; j++) {
            if(i < n) A[n*(i-1)+j-1][n*i+j-1] = (i-k)*(i-k) + (j-k)*(j-k) + (i-k) + c/(4*dx*dx);
            if(i > 1) A[n*(i-1)+j-1][n*(i-2)+j-1] = (i-k)*(i-k) + (j-k)*(j-k) - (i-k) + c/(4*dx*dx);
            A[n*(i-1)+j-1][n*(i-1)+j-1] = -4*((i-k)*(i-k) + (j-k)*(j-k) + c/(4*dx*dx));
            if(j < n) A[n*(i-1)+j-1][n*(i-1) + j] = (i-k)*(i-k) + (j-k)*(j-k) + (j-k) + c/(4*dx*dx);
            if(j > 1) A[n*(i-1)+j-1][n*(i-1) + j-2] = (i-k)*(i-k) + (j-k)*(j-k) - (j-k) + c/(4*dx*dx);
        }
    }
    // for(i=0; i<n*n; i++) {
    //     for(j=0; j<n*n; j++) {
    //         cout << A[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    for(i=1; i<=n; i++) {
        for(j=1; j<=n; j++) {
            if(i == n) b[n*(i-1)+j-1] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (i-k) + c/(4*dx*dx))*T1;
            if(i == 1) b[n*(i-1)+j-1] = -1*((i-k)*(i-k) + (j-k)*(j-k) - (i-k) + c/(4*dx*dx))*T1;
            if(j == 1) b[n*(i-1)+j-1] = -1*((i-k)*(i-k) + (j-k)*(j-k) - (j-k) + c/(4*dx*dx))*T1;
            if(j == n) b[n*(i-1)+j-1] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (j-k) + c/(4*dx*dx))*T2;
            if(i == n && j == n) b[n*(i-1)+j-1] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (i-k) + c/(4*dx*dx))*T1 + -1*((i-k)*(i-k) + (j-k)*(j-k) + (j-k) + c/(4*dx*dx))*T2;
            if(i == n && j == 1) b[n*(i-1)+j-1] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (i-k) + c/(4*dx*dx))*T1 + -1*((i-k)*(i-k) + (j-k)*(j-k) - (j-k) + c/(4*dx*dx))*T1;
            if(j == 1 && i == 1) b[n*(i-1)+j-1] = -1*((i-k)*(i-k) + (j-k)*(j-k) - (j-k) + c/(4*dx*dx))*T1 + -1*((i-k)*(i-k) + (j-k)*(j-k) - (i-k) + c/(4*dx*dx))*T1;
            if(j == n && i == 1) b[n*(i-1)+j-1] = -1*((i-k)*(i-k) + (j-k)*(j-k) + (j-k) + c/(4*dx*dx))*T2 + -1*((i-k)*(i-k) + (j-k)*(j-k) - (i-k) + c/(4*dx*dx))*T1;
        }
    }
    // for(i=0; i<n; i++) {
    //     for(j=0; j<n; j++) {
    //         cout << b[n*i + j] << " ";
    //     }
    //     cout << endl;
    // }
    auto start = high_resolution_clock::now();
    luDecomposition(A,b,x,n*n);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "LU Decomposition - " << (ld)duration.count()/1000000 << " s" << endl;
    // to get .dat file
    // for(i=-1; i<=n; i++) {
    //     for(j=-1; j<=n; j++) {
    //         if(j==n) {
    //             cout << (i+1)*dx << "\t" << (j+1)*dx << "\t" << T2 << endl;
    //         }
    //         else if(i==-1) {
    //             cout << (i+1)*dx << "\t" << (j+1)*dx << "\t" << T1 << endl;
    //         }
    //         else if(j==-1) {
    //             cout << (i+1)*dx << "\t" << (j+1)*dx << "\t" <<  T1 << endl;
    //         }
    //         else if(i==n) {
    //             cout << (i+1)*dx << "\t" << (j+1)*dx << "\t" << T1 << endl;
    //         }
    //         else if(i!=-1 && i!=n && j!=-1 && j!=n) {
    //             cout << (i+1)*dx << "\t" << (j+1)*dx << "\t" << x[n*i + j] << endl;
    //         }
    //     }
    // }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            cout << x[n*i + j] << " ";
        }
        cout << endl;
    }
    for(i=0; i<n*n; i++) {
        delete A[i];
    }
    delete A;
    delete x;
    delete b;
    return 0;
}