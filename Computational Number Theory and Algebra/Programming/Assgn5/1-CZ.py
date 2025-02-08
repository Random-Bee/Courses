import csv
import os
import random

def exp(a, b, p):
    e = 1
    while b:
        if b&1:
            e = (e*a)%p
        a = (a*a)%p
        b >>= 1
    return e

def printPoly(f):
    if f == [0]:
        print("(0)")
        return
    d = len(f) - 1
    print("(", end="")
    for i in range(d+1):
        if(f[i] != 0):
            if i != 0:
                print(" + ", end="")
            if d-i == 0:
                print(f"{f[i]}", end="")
            elif(f[i] != 1):
                if d-i == 1:
                    print(f"{f[i]}*x", end="")
                else:
                    print(f"{f[i]}*x^{d-i}", end="")
            else:
                if d-i == 1:
                    print("x", end="")
                else:
                    print(f"x^{d-i}", end="")
    print(")", end="")
    return

# add f and g
def polyAdd(f, g, p):
    fc = f.copy()
    gc = g.copy()
    while len(fc) < len(gc):
        fc.insert(0, 0)
    while len(gc) < len(fc):
        gc.insert(0, 0)
    h = [(fc[i] + gc[i])%p for i in range(len(fc))]
    while h[0] == 0 and len(h) > 1:
        h.pop(0)
    return h

# multiply f and g
def polyMul(f, g, p):
    h = [0 for i in range(len(f)+len(g)-1)]
    for i in range(len(f)):
        for j in range(len(g)):
            h[i+j] = (h[i+j] + f[i]*g[j])%p
    while h != [] and h[0] == 0:
        h.pop(0)
    if h == []:
        h = [0]
    return h

# divide f by g
def polyDiv(f, g, p):
    quo = [0 for i in range(len(f)+1)]
    rem = f.copy()
    while len(rem) >= len(g) and rem != [0]:
        d = len(rem) - len(g)
        c = (rem[0]*exp(g[0], p-2, p))%p
        quo[len(quo)-d-1] = c
        for i in range(d):
            g.append(0)
        rem = [(rem[i] - c*g[i])%p for i in range(len(rem))]
        for i in range(d):
            g.pop()
        while rem[0] == 0 and len(rem) > 1:
            rem.pop(0)
    while quo != [] and quo[0] == 0:
        quo.pop(0)
    if quo == []:
        quo = [0]
    return quo, rem

# multiply matrices A and B, each element is a polynomial
def matMul(A, B, p):
    C = [[[0] for i in range(len(B[0]))] for j in range(len(A))]
    for i in range(len(A)):
        for j in range(len(B[0])):
            for k in range(len(B)):
                C[i][j] = polyAdd(C[i][j], polyMul(A[i][k], B[k][j], p), p)
    return C

def extendedPolyGcd(f, g, p):
    fc = f.copy()
    gc = g.copy()
    Q = [[[1], [0]], [[0], [1]]]
    while fc != [0]:
        q, r = polyDiv(gc, fc, p)
        fc, gc = r, fc
        q_ = [(-1*q[i])%p for i in range(len(q))]
        Q = matMul([[q_, [1]], [[1], [0]]], Q, p)

    # f*u + g*v = gcd(f, g)
    u = Q[1][0]
    v = Q[1][1]
    gcd = polyAdd(polyMul(f, u, p), polyMul(g, v, p), p)
    
    # Make gcd monic
    c = exp(gcd[0], p-2, p)
    gcd = [(c*gcd[i])%p for i in range(len(gcd))]
    u = [(c*u[i])%p for i in range(len(u))]
    v = [(c*v[i])%p for i in range(len(v))]
    return gcd, u, v

# Phase 1: Finding the square-free part

# Derivative of f
def deriv(f, p):
    df = []
    for i in range(0, len(f)-1):
        df.append(((len(f)-1-i)*f[i])%p)
    while df != [] and df[0] == 0:
        df.pop(0)
    if df == []:
        df = [0]
    return df

def squareFreePart(f, p):
    g = f.copy()
    F = f.copy()
    while g != [0]:
        h = g
        g = deriv(g, p)
    if len(h) < p:
        df = deriv(f, p)
        gcd, _, _ = extendedPolyGcd(f, df, p)
        return polyDiv(f, gcd, p)[0]
    F = polyDiv(F, h, p)[0]
    h_ = [0 for i in range(len(h))]
    for i in range(len(h)):
        if h[i] != 0:
            curr_deg = len(h) - 1 - i
            new_deg = curr_deg//p
            h_[len(h)-1-new_deg] = h[i]
    h = h_
    while h != [] and h[0] == 0:
        h.pop(0)
    if h == []:
        h = [0]
    h = squareFreePart(h, p)
    dF = deriv(F, p)
    gcd, _, _ = extendedPolyGcd(F, dF, p)
    return polyMul(h, polyDiv(F, gcd, p)[0], p)

# Phase 2: Distinct-degree factorization

# gcd of f and x^(p^i) - x
def modifiedGCD(fc, i, p):
    f = fc.copy()
    # find x^(p^i) % f using repeated squaring
    h = [1, 0] # x
    g = [1] # 1
    mx = p**i
    while mx:
        if mx&1:
            g = polyMul(g, h, p)
            g = polyDiv(g, f, p)[1]
        h = polyMul(h, h, p)
        h = polyDiv(h, f, p)[1]
        mx >>= 1
    # Add -x to x^(p^i)
    g = polyAdd(g, [-1, 0], p)
    g = polyDiv(g, f, p)[1]
    return extendedPolyGcd(g, f, p)[0]

def distinctDegreeFactorization(fc, p):
    i = 1
    f = fc.copy()
    g = f
    factors = {}
    while g != [1]:
        factor = modifiedGCD(g, i, p)
        if factor != [1]:
            factors[i] = modifiedGCD(g, i, p)
            g = polyDiv(g, factors[i], p)[0]
        i += 1
    return factors


# Phase 3: Finding irreducible factors of degree i

# gcd of f and g^((p^i-1)/2) - 1
def modifiedGCD2(fc, gc, i, p):
    f = fc.copy()
    g = gc.copy()
    # find g^(p^i-1)/2 % f using repeated squaring
    mx = (p**i-1)//2
    h = [1]
    while mx:
        if mx&1:
            h = polyMul(h, g, p)
            h = polyDiv(h, f, p)[1]
        g = polyMul(g, g, p)
        g = polyDiv(g, f, p)[1]
        mx >>= 1
    # Add -1 to g^(p^i-1)/2
    h = polyAdd(h, [-1], p)
    h = polyDiv(h, f, p)[1]
    return extendedPolyGcd(h, f, p)[0]

def irreducibleFactors(f, i, p):
    if len(f) == i+1:
        return f
    h1 = [1]
    h2 = [1]
    while h1 == [1] or h2 == [1]:
        g = [random.randint(1, p-1) for j in range(len(f)-1)]
        h1 = modifiedGCD2(f, g, i, p)
        h2 = polyDiv(f, h1, p)[0]
    list1 = irreducibleFactors(h1, i, p)
    list2 = irreducibleFactors(h2, i, p)
    return list1 + list2

# Combine all phases
def getAllFactors(f, p):
    sfp = squareFreePart(f, p)
    factors = distinctDegreeFactorization(sfp, p)
    irrFactors = {}
    for i in factors:
        irrFactors[i] = irreducibleFactors(factors[i], i, p)
    allFactors = []
    for i in irrFactors:
        for j in range(0, len(irrFactors[i]), i+1):
            g = []
            for k in range(j, j+i+1):
                g.append(irrFactors[i][k])
            F = f.copy()
            while polyDiv(F, g, p)[1] == [0]:
                allFactors.append(g)
                F = polyDiv(F, g, p)[0]
    for i in range(len(allFactors)):
        printPoly(allFactors[i])
        if i != len(allFactors) - 1:
            print(" * ", end="")
    print()
    print()
    return

def main():
    inputFile = "input-CZ.csv"
    if os.path.exists(inputFile) == False:
        print("File not found!")
        return
    with open(inputFile, 'r') as file:
        reader = csv.reader(file)
        p = int(next(reader)[0])
        while True:
            try:
                str_f = next(reader)
                deg_f = int(str_f[0])
                f = [int(str_f[i]) for i in range(1, deg_f + 2)]
                getAllFactors(f, p)
            except StopIteration:
                break
    return

main()