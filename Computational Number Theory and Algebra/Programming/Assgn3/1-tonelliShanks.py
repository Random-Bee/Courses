import csv

def exp(a, b, p):
    e = 1
    while b:
        if b&1:
            e = (e*a)%p
        a = (a*a)%p
        b >>= 1
    return e

def findk(b, p):
    k = 0
    while exp(b, exp(2, k, p-1), p) != 1:
        k += 1
    return k

def tonelliShanks(a, p):
    if exp(a, (p-1)//2, p) != 1:
        return 0
    m = p-1
    t = 0
    while m%2 == 0:
        m //= 2
        t += 1
    b = exp(a, m, p)
    k = findk(b, p)
    if k == t:
        return 0
    x = exp(a, (m+1)//2, p)
    if k == 0:
        return min(x, p-x)
    r = 2
    while exp(r, (p-1)//2, p) == 1:
        r += 1
    s = exp(r, m, p)
    S = exp(s, 2**(t-k), p)
    while k:
        b = (b*S)%p
        x = (x*exp(s, 2**(t-k-1), p))%p
        k = findk(b, p)
        S = exp(s, 2**(t-k), p)
    return min(x, p-x)

def main():
    with open('inputSquareRoots.csv', 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            a, p = int(row[0]), int(row[1])
            x = tonelliShanks(a, p)
            print(x)
    return

main()