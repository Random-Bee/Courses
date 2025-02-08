#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;
// Question 2

// dx/dt = f2(x,r,k)
ld f2(ld x, ld r, ld k) {
    return r*x*(1 - x/k) - (x*x)/(1 + x*x);
}

void solve2() {
    ld ti, h = 0.1, r = 0.4, k = 20, eps = 1e-8, f_prev;
    // taking timestep as 0.001

    vector<ld> values_a, values_b;

    // part a
    ti = 0; // initial time
    values_a.push_back(2.44); // initial population
    while(true) {
        ti += h;
        f_prev = values_a.back();
        values_a.push_back(values_a.back() + h*f2(values_a.back(), r, k)); // Euler's first order scheme
        if(abs(f_prev-values_a.back())<eps) break; // if converged, break
    }
    cout << setprecision(9);
    // print the results
    for(int i=0; i<values_a.size(); i++) {
        cout << values_a[i] << ", ";
    }
    cout << endl;
    cout << "A - " << values_a.back() << endl; // print the final population

    // part b
    ti = 0; // initial time
    values_b.push_back(2.4); // initial population
    while(true) {
        ti += h;
        f_prev = values_b.back();
        values_b.push_back(values_b.back() + h*f2(values_b.back(), r, k)); // Euler's first order scheme
        if(abs(f_prev-values_b.back())<eps) break; // if converged, break 
    }
    // print the results
    for(int i=0; i<values_b.size(); i++) {
        cout << values_b[i] << ", ";
    }
    cout << endl;
    cout << "B - " << values_b.back() << endl; // print the final population
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    solve2();
    return 0;
}