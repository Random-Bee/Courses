#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
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
    ld x1[n],m,w=1,z;
    for(i=0; i<n; i++) x[i] = 50; //initial guess => x[i] = 50
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
            z /= A[i][i];
            x[i] = (1-w)*x[i] + w*z;
        }
        k++;
        cout << k << " ";
        if(check(A,b,x,x1,n,s,&m,&w)) {
            break;
        }
    }
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
    gaussSeidel(A,b,x,n*n,s);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Gauss Seidel - " << (ld)duration.count()/1000000 << " s" << endl;
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