#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;

// Question 7

// du2/dt = f7a(u1, u2)
ld f7a(ld u1, ld u2) {
    return (1-u1*u1)*u2 - u1;
}

// du1/dt = f7b(u2)
ld f7b(ld u2) {
    return u2;
}

void solve7() {
    ld ti = 0.0, u1_0 = 0.5, u2_0 = 0.1, h = 0.1, u1_old, u2_old, u1_p, u2_p, k1, k2, k3, k4, l1, l2, l3, l4;
    vector<ld> u1,u2;
    u1.push_back(u1_0);
    u2.push_back(u2_0);
    ll it=0;

    // x'' + (x^2-1)*x' + x = 0
    // putting u1 = x, u2 = x', we get:
    // u1' = u2                           --(1)
    // u2' + (u1^2-1)*u2 + u1 = 0         --(2)
    // Solve (1) & (2) simultaneously using RK-4

    while(it<1000) {
        it++;
        ti += h;
        u1_old = u1.back(); u2_old = u2.back();

        // Using RK-4 simultaneously for system of two equations

        k1 = h*(f7b(u2_old));
        l1 = h*(f7a(u1_old, u2_old));

        k2 = h*(f7b(u2_old+l1/2));
        l2 = h*(f7a(u1_old+k1/2, u2_old+l1/2));

        k3 = h*(f7b(u2_old+l2/2));
        l3 = h*(f7a(u1_old+k2/2, u2_old+l2/2));
        
        k4 = h*(f7b(u2_old+l3));
        l4 = h*(f7a(u1_old+k3/2, u2_old+l3/2));

        u2_p = u2_old + (l1+2*l2+2*l3+l4)/6; // New predicted value of u2
        u1_p = u1_old + (k1+2*k2+3*k3+k4)/6; // New predicted value of u1

        u2.push_back(u2_p);
        u1.push_back(u1_p);

        // Number of iterations for first 4-cycles found from plotting large number of iterations.
        cout << u1.back() << ", ";
        if(it>295) {
            break;
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    solve7();
    return 0;
}