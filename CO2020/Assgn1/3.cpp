#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;

// Question 3

ld Cp(ld T) {
    return 0.162*T + 446.47;
}

// dT/dt = f3(T,Tf,t,rho,d)
ld f3(ld T, ld Tf, ld t, ld rho, ld d) {
    return (2*5.67*pow(10,-8)*(pow(Tf,4)-pow(T,4)))/(rho*d*(Cp(T)+t*0.162));
}

void solve3() {
    ld Tf = 1500, T, ti=0, d = 0.002, rho = 7900, k1, k2, k3, k4, h=0.1, eps=1e-8;
    vector<ld> values;
    values.push_back(300.0); // Initial temperature
    while(abs(Tf-values.back())>eps) { // While not converged
        k1 = h*f3(values.back(), Tf, ti, rho, d);
        k2 = h*f3(values.back()+k1/2, Tf, ti+h/2, rho, d);
        k1 = h*f3(values.back()+k2/2, Tf, ti+h/2, rho, d);
        k4 = h*f3(values.back()+k3, Tf, ti+h, rho, d);
        values.push_back(values.back() + (k1 + 2*k2 + 2*k3 + k4)/6); // Update T using RK-4 method
        ti += h;
    }
    // Print the results
    cout << setprecision(15);
    for(int i=0; i<values.size(); i++) {
        cout << values[i] << ", ";
    }
    cout << endl;
    cout << ti << endl; // Print final time
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    solve3();
    return 0;
}