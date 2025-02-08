import csv

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

        print(Q)
    
    return Q[1]

def main():
    with open('testinput-gcd.csv', 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            a, b = int(row[0]), int(row[1])
            x, y = extendedEuclid(a, b)
            print(f'x={x},y={y},c={a*x+b*y}')

main()