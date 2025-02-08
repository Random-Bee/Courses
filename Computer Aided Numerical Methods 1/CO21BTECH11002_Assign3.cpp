#include <bits/stdc++.h>
using namespace std;
long double** stackMat(long double** A, int n) {
    long double** E;
    int i,j;
    E = new long double*[n];
    for (i=0; i<n; i++) {
        E[i] = new long double[2*n];
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            E[i][j] = A[i][j];
        }
    }
    for(i=0; i<n; i++) {
        for(j=n; j<2*n; j++) {
            if(i+n==j) E[i][j] = 1;
            else E[i][j] = 0;
        }
    }
    return E;
}

long double** destack(long double** A, int n) {
    long double** E;
    int i,j;
    E = new long double*[n];
    for (i=0; i<n; i++) {
        E[i] = new long double[2*n];
    }
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            E[i][j] = A[i][j+n];
        }
    }
    return E;
}

// Add m times row j to row i in a square matrixmatrix A
void add_row(long double** A, long double m, int i, int j, int n) {
    int k;
    for(k=0; k<2*n; k++) {
        A[i][k] += m*A[j][k];
    }
}

void scale_row(long double** A, long double s, int i, int n) {
    int k;
    for(k=0; k<2*n; k++) {
        A[i][k] *= s;
    }
}

void swap_rows(long double** A, int i, int j, int n) {
    int k;
    double temp;
    for(k=0; k<2*n; k++) {
        temp = A[i][k];
        A[i][k] = A[j][k];
        A[j][k] = temp;
    }
}

int findmax(long double** A, int i, int j, int n) {
    long double maxval = A[i][j];
    int index = i, p;
    for (p=i; p<n; p++) {
        if (A[p][j] > maxval) {
            maxval = A[p][j];
            index = p;
        }
    }
    return index;
}

int findmin(long double** A, int i, int j, int n) {
    long double minval = A[i][j];
    int index = i, p;
    for (p=i; p<n; p++) {
        if (A[p][j] < minval) {
            minval = A[p][j];
            index = p;
        }
    }
    return index;
}

long double** inverse(long double** A, int m, int n) {
    int det1 = 1, c = 1, index, i, j, k, l;
    long double **arr, **inv;
    if (m != n) {
        cout << "Enter a valid n*n matrix.\n";
        return NULL;
    }
    else {
        arr = stackMat(A,n);
        for (i=0; i<n-1; i++) {
            //pivoting
            index = findmax(arr,i,i,n);
            if (arr[index][i] == 0) {
                index = findmin(arr,i,i,n);
                if (arr[index][i] == 0) {
                    cout << "Enter a valid non-singular matrix.\n";
                    return NULL;
                }
            }
            if (index != i) {
                swap_rows(arr,i,index,n);
            }
            for (j=i+1; j<n; j++) {
                add_row(arr,-1*arr[j][j-c]/arr[i][i],j,i,n);
                c++;
            }
            c = 1;
        }
        if (arr[n-1][n-1] == 0) {
            cout << "Enter a valid non-singular matrix.\n";
            return NULL;
        }
        for (i=n-1; i>0; i--) {
            for (j=i-1; j>-1; j--) {
                add_row(arr,-1*arr[j][j+c]/arr[i][i],j,i,n);
                c++;
            }
            c = 1;
        }
        for (i=0; i<n; i++) {
            scale_row(arr,1/arr[i][i],i,n);
        }
        inv = destack(arr,n);
        for(i=0; i<n; i++) {
            delete arr[i];
        }
        delete arr;
        return inv;
    }
}

long double** multiplyMat(long double** A, long double** B, int a, int b, int c) {
    int i,j,k;
    long double s=0,**M;
    M = new long double*[a];
    for (i=0; i<a; i++) {
        M[i] = new long double[c];
    }
    for (i = 0; i < a; i++) {
        for (j = 0; j < c; j++) {
            for (k = 0; k < b; k++) {
                s += A[i][k] * B[k][j];
            }
            M[i][j] = s;
            s = 0;
        }
    }
    return M;
}

int main() {
    int m,n,i,j,k=0;
    long double **A, **inv, **x, **B;
    cin >> m >> n;
    A = new long double*[m];
    for (i=0; i<m; i++) {
        A[i] = new long double[n];
    }
    B = new long double*[m];
    for (i=0; i<m; i++) {
        B[i] = new long double[1];
    }
    for(i=0; i<m; i++) {
        for(j=0; j<n; j++) {
            A[i][j] = rand()%9;
        }
    }
    for(i=0; i<m; i++) {
        for(j=0; j<1; j++) {
            B[i][j] = rand()%9;
        }
    }
    for(i=0; i<m; i++) {
        for(j=0; j<n; j++) {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
    for(i=0; i<m; i++) {
        for(j=0; j<1; j++) {
            cout << B[i][j] << " ";
        }
        cout << endl;
    }
    inv = inverse(A,m,n);
    if(inv != NULL) {
        for(i=0; i<m; i++) {
            for(j=0; j<n; j++) {
                cout << inv[i][j] << " ";
            }
            cout << endl;
        }
        x = multiplyMat(inv,B,m,n,1);
        for(i=0; i<m; i++) {
            for(j=0; j<1; j++) {
                cout << x[i][j] << " ";
            }
            cout << endl;
        }
    }
    for(i=0; i<m; i++) {
        delete A[i];
        delete B[i];
    }
    if(inv != NULL) {
        for(i=0; i<m; i++) {
            delete inv[i];
            delete x[i];
        }
    }
    delete A;
    delete inv;
    delete x;
    delete B;
    return 0;
}