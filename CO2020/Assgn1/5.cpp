#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;

// Question 5

// dx/dt = f5(x,t)
ld f5(ld x, ld t) {
    return (t*x*x - x)/t;
}

// Actual function
ld x5(ld t) {
    return -1/(t*log(2*t));
}

void solve5() {
    ld k1, k2, ti, f1 = -1/log(2), tn=5.0, h=0.1;
    vector<ld> valuesRK2, valuesABM;

    // RK2
    ti = 1.0; // initial time
    valuesRK2.push_back(f1);
    while(ti<tn) {
        k1 = h*f5(valuesRK2.back(), ti);
        k2 = h*f5(valuesRK2.back()+k1, ti+h);
        valuesRK2.push_back(valuesRK2.back() + (k1+k2)/2);
        ti+=h;
    }
    cout << setprecision(10);


    // ABM
    ti = 1; // initial time
    ll n = 2;
    ld fp, fc; // f_predicted & f_corrected
    // taking first two values from RK-2
    valuesABM.push_back(valuesRK2[0]);
    ti += h;
    valuesABM.push_back(valuesRK2[1]);
    ti += h;
    while(ti<=tn+h) {
        fp = valuesABM[n-1] + (h/2)*(3*f5(valuesABM[n-1], ti-h) - f5(valuesABM[n-2], ti-h*2));
        fc = valuesABM[n-1] + (h/2)*(f5(fp, ti) + f5(valuesABM[n-1], ti-h));
        valuesABM.push_back(fc);
        ti += h;
        n++;
    }

    cout << valuesABM.size() << " "<< valuesRK2.size() << endl;

    // Print the results
    cout << setprecision(10);
    for(int i=0; i<valuesRK2.size(); i++) {
        cout << valuesRK2[i] << ", ";
    }
    cout << endl;
    cout << "Percentage error from RK-2: " << (x5(tn)-valuesRK2.back())*100/x5(tn) << endl;
    for(int i=0; i<valuesABM.size(); i++) {
        cout << valuesABM[i] << ", ";
    }
    cout << endl;
    cout << "Percentage error from ABM-4: " << (x5(tn)-valuesABM.back())*100/x5(tn) << endl;
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    solve5();
    return 0;
}