#include <bits/stdc++.h>
using namespace std;
typedef long double ld;
ld f(ld x) {
    return exp(0.8*x)-1;
}
ld interpolateLG(ld x, ld h, int o) {
    int i,j,k=0;
    ld pkv[o+1],w,s;
    for(i=0; i<=o; i++) {
        if(i==o) k = 1;
        pkv[i] = x-h*(o-i-0.5);
    }
    s = 0;
    for(i=0; i<=o; i++) {
        w = 1;
        for(j=0; j<=o; j++) {
            if(j != i) {
                w *= (x - pkv[j])/(pkv[i] - pkv[j]);
            }
        }
        s += w*f(pkv[i]);
    }
    cout << fixed;
    cout << setprecision(10);
    cout << "Value - " << s << endl << "\%Error - " << (f(x)-s)*100/f(x) << endl;
    return s;
}
ld interpolateNP(ld x, ld h, int o) {
    int i,j,k=0,d;
    ld pkv[o+1],w,s,y[o+1][o+1];
    for(i=0; i<=o; i++) {
        pkv[i] = x-h*(o-i-0.5);
    }
    for(i=0; i<=o; i++) {
        for(j=0; j<=o; j++) {
            y[i][j] = 0;
        }
    }
    for(i=0; i<=o; i++) {
        y[i][0] = f(pkv[i]);
    }
    for(j=1; j<=o; j++) {
        for(i=0; i<=o-j; i++) {
            y[i][j] = (y[i+1][j-1]-y[i][j-1]);
        }
    }
    d=1;
    for(i=1; i<=o; i++) {
        d *= i;
        y[0][i] /= d;
    }
    s = y[0][o];
    for(i=0; i<o; i++) {
        s *= (x-pkv[o-i-1])/h;
        s += y[0][o-i-1];
    }
    cout << fixed;
    cout << setprecision(10);
    cout << "Value = " << s << endl << "\%Error = " << (f(x)-s)*100/f(x) << endl;
    return s;
}
int main() {
    int i,j,k,n,o;
    ld h,x;
    // f(x) = e^(0.8x) - 1;
    cout << "Enter the number of tests: ";
    cin >> n;
    for(i=0; i<n; i++) {
        cout << endl << "Enter the value of  x : ";
        cin >> x;
        cout << endl;
        cout << "Using Lagrange Polynomial:" << endl << endl;
        cout << "Using 2nd degree polynomial:" << endl;
        o = 2;
        cout << "For h = 0.1:" << endl;
        h = 0.1;
        interpolateLG(x,h,o);
        cout << "For h = 0.05:  " << endl;
        h = 0.05;
        interpolateLG(x,h,o);
        cout << endl << "Using 4th degree polynomial: " << endl;
        o = 4;
        cout << "For h = 0.1:" << endl;
        h = 0.1;
        interpolateLG(x,h,o);
        cout << "For h = 0.05:  " << endl;
        h = 0.05;
        interpolateLG(x,h,o);
        cout << endl;
        cout << "Using Newton's Polynomial:" << endl << endl;
        cout << "Using 2nd degree polynomial:" << endl;
        o = 2;
        cout << "For h = 0.1:  " << endl;
        h = 0.1;
        interpolateNP(x,h,o);
        cout << "For h = 0.05:  "<< endl;
        h = 0.05;
        interpolateNP(x,h,o);
        cout << endl << "Using 4th degree polynomial: " << endl;
        o = 4;
        cout << "For h = 0.1:  " << endl;
        h = 0.1;
        interpolateNP(x,h,o);
        cout << "For h = 0.05:  " << endl;
        h = 0.05;
        interpolateNP(x,h,o);
    }
    return 0;
}