#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;

// Question 1

// dx/dt = f1(x,t)
ld f1(ld x, ld t) {
    return -(1+t+t*t)-(2*t+1)*x-x*x;
}

// exact function in part a
ld x1a(ld t) {
    return -t - 1.0/(exp(t)+1);
}

// exact function in part b
ld x1b(ld t) {
    return -t-1;
}

void solve1() {
    ld ti, tn = 3.0, x0 = -0.5, h = 0.1;

    vector<ld> valuesa, valuesb;

    // part a
    x0 = -0.5; // initial value of x
    ti = 0; // initial time
    valuesa.push_back(x0);
    while(ti<tn) {
        ti += h;
        valuesa.push_back(valuesa.back() + h*f1(valuesa.back(), ti-h)); // Euler's first order scheme
    }

    // part b
    x0 = -1; // initial value of x
    ti = 0; // initial time
    valuesb.push_back(x0);
    while(ti<tn) {
        ti += h;
        valuesb.push_back(valuesb.back() + h*f1(valuesb.back(), ti-h)); // Euler's first order scheme
    }

    // print the output
    for(int i=0; i<valuesa.size(); i++) {
        cout << valuesa[i] << ", ";
    }
    cout << endl;
    for(int i=0; i<valuesb.size(); i++) {
        cout << valuesb[i] << ", ";
    }
    cout << endl;
}

int main() {
    solve1();
    return 0;
}