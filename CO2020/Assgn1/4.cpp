#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;

// Question 4

// dx/dt = f4(x,t)
ld f4(ld x, ld t) {
    return 7*t*t - (4*x)/t;
}

// actual function
ld x4(ld t) {
    return t*t*t + 1/(t*t*t*t);
}

void solve4() {
    ld k1, k2, k3, k4, ti, f1 = 2.0, tn=6.0, h=0.1;
    vector<ld> values2, values4;

    // RK2
    ti = 1.0;
    values2.push_back(f1);
    while(ti<tn) {
        k1 = h*f4(values2.back(), ti);
        k2 = h*f4(values2.back()+k1, ti+h);
        values2.push_back(values2.back() + (k1+k2)/2);
        ti+=h;
    }

    // RK4
    ti = 1.0;
    values4.push_back(f1);
    while(ti<tn) {
        k1 = h*f4(values4.back(), ti);
        k2 = h*f4(values4.back()+k1/2, ti+h/2);
        k3 = h*f4(values4.back()+k2/2, ti+h/2);
        k4 = h*f4(values4.back()+k3, ti+h);
        values4.push_back(values4.back() + (k1 + 2*k2 + 2*k3 + k4)/6);
        ti += h;
    }

    // Print the results
    cout << setprecision(10);
    for(int i=0; i<values2.size(); i++) {
        cout << values2[i] << ", ";
    }
    cout << endl;
    cout << "Percentage error from RK-2: " << (x4(tn)-values2.back())*100/x4(tn) << endl;
    for(int i=0; i<values4.size(); i++) {
        cout << values4[i] << ", ";
    }
    cout << endl;
    cout << "Percentage error from RK-4: " << (x4(tn)-values4.back())*100/x4(tn) << endl;
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    solve4();
    return 0;
}