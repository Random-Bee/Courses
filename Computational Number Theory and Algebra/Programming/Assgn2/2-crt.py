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

def crt():
    with open('testinput-crt.txt', 'r') as f:
        for line in f:
            a, m, b, n = map(int, line.strip().split(','))
            if gcd(m, n) != 1:
                print(-1)
                continue
            sol = extendedEuclid(m, -n)
            sol[0] *= (b-a)
            sol[1] *= (b-a)
            print((a + m*sol[0]) % (m*n))

crt()