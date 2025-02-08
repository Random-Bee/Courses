#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;

// Question 6

// dx/dt = f6(x,t)
ld f6(ld x, ld t) {
    return (4*x)/t + pow(t,4)*exp(t);
}

// actual function
ld x6(ld t) {
    return pow(t,4)*(exp(t)-exp(1));
}

void solve6() {
    ld k1, k2, k3, k4, ti, f1 = 0, tn=2, h=0.05;
    vector<ld> valuesRK4, valuesABM;

    // RK2
    ti = 1.0; // initial time
    valuesRK4.push_back(f1);
    while(ti<tn) {
        k1 = h*f6(valuesRK4.back(), ti);
        k2 = h*f6(valuesRK4.back()+k1/2, ti+h/2);
        k3 = h*f6(valuesRK4.back()+k2/2, ti+h/2);
        k4 = h*f6(valuesRK4.back()+k3, ti+h);
        valuesRK4.push_back(valuesRK4.back() + (k1 + 2*k2 + 2*k3 + k4)/6);
        ti += h;
    }
    // ABM
    ti = 1.0; // initial time
    ld fp, fc; //f_predicted, f_corrected
    // taking first four values from RK-4
    valuesABM.push_back(valuesRK4[0]);
    ti += h;
    valuesABM.push_back(valuesRK4[1]);
    ti += h;
    valuesABM.push_back(valuesRK4[2]);
    ti += h;
    valuesABM.push_back(valuesRK4[3]);
    ti += h;
    ll n = 4; // current number of points that are already known
    while(ti<=tn+h) {
        fp = valuesABM[n-1] + (h/24)*(55*f6(valuesABM[n-1], ti-h) - 59*f6(valuesABM[n-2], ti-h*2) + 37*f6(valuesABM[n-3], ti-h*3) -9*f6(valuesABM[n-4], ti-h*4));
        fc = valuesABM[n-1] + (h/24)*(9*f6(fp, ti) + 19*f6(valuesABM[n-1], ti-h) - 5*f6(valuesABM[n-2], ti-h*2) + f6(valuesABM[n-3], ti-h*3));
        valuesABM.push_back(fc);
        ti += h;
        n++;
    }

    // Print the results
    cout << setprecision(10);
    for(int i=0; i<valuesRK4.size(); i++) {
        cout << valuesRK4[i] << ", ";
    }
    cout << endl;
    cout << "Percentage error from RK-4: " << (x6(tn)-valuesRK4.back())*100/x6(tn) << endl;
    for(int i=0; i<valuesABM.size(); i++) {
        cout << valuesABM[i] << ", ";
    }
    cout << endl;
    cout << "Percentage error from ABM-4: " << (x6(tn)-valuesABM.back())*100/x6(tn) << endl;
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    solve6();
    return 0;
}