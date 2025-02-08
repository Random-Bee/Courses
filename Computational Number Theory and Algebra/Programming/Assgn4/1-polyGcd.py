import csv
import os

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
        print("0")
        return
    d = len(f) - 1
    for i in range(d+1):
        if(f[i] != 0):
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
            if i != d:
                print(" + ", end="")
    print()
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
    while h[0] == 0 and len(h) > 1:
        h.pop(0)
    return h

# divide f by g
def polyDiv(f, g, p):
    quo = []
    rem = f.copy()
    while len(rem) >= len(g) and rem != [0]:
        d = len(rem) - len(g)
        c = (rem[0]*exp(g[0], p-2, p))%p
        quo.append(c)
        for i in range(d):
            g.append(0)
        rem = [(rem[i] - c*g[i])%p for i in range(len(rem))]
        for i in range(d):
            g.pop()
        while rem[0] == 0 and len(rem) > 1:
            rem.pop(0)
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

def main():
    inputFile = "input-polygcd2.csv"

    if os.path.exists(inputFile) == False:
        print("File not found!")
        return
    
    with open(inputFile, 'r') as file:
        reader = csv.reader(file)
        p = int(next(reader)[0])
        str_f = next(reader)
        str_g = next(reader)
        deg_f = int(str_f[0])
        deg_g = int(str_g[0])
        f = [int(str_f[i]) for i in range(1, deg_f + 2)]
        g = [int(str_g[i]) for i in range(1, deg_g + 2)]
        gcd, u, v = extendedPolyGcd(f, g, p)
        print("GCD: ", end="")
        printPoly(gcd)
        print("u: ", end="")
        printPoly(u)
        print("v: ", end="")
        printPoly(v)
    return

main()