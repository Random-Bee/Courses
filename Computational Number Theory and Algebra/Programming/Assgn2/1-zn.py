def matMul(A, B):
    C = [[0 for i in range(len(B[0]))] for j in range(len(A))]
    for i in range(len(A)):
        for j in range(len(B[0])):
            for k in range(len(B)):
                C[i][j] += A[i][k] * B[k][j]
    return C

def extendedEuclid(a, b):
    Q = [[1, 0], [0, 1]]

    while(a != 0):
        q = b // a
        a, b = b % a, a
        Q = matMul([[-q, 1], [1, 0]], Q)
    
    return Q[1]

def gcd(a, b):
    if(a>b):
        a, b = b, a
    while a:
        a, b = b%a, a
    return b

def exp(a, b, n):
    e = 1
    while b:
        if b&1:
            e = (e*a)%n
        a = (a*a)%n
        b >>= 1
    return e

def zn():
    with open('testinput-Zn.txt', 'r') as f:
        for line in f:
            n, a, b = map(int, line.strip().split(','))
            ex = exp(a, b, n)
            if(gcd(a, n) == 1):
                inv = extendedEuclid(a, -n)[0]
                if(inv < 0):
                    inv = inv%n
                print(f'{ex},true,{inv}')
            else:
                print(f'{ex},false')

zn()