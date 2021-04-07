#!/usr/bin/python3

class Rational():
    def __init__(self, num, den):
        self.num = num
        self.den = den

    def __add__(self, other):
        return Rational(self.num * other.den + self.den * other.num, self.den * other.den)
    
    def __neg__(self):
        return Rational(-self.num, self.den)
    
    def __sub__(self, other):
        return self + (-other)
    
    def __mul__(self, other):
        return Rational(self.num * other.num, self.den * other.den)

    def __truediv__(self, other):
        return Rational(self.num * other.den, self.den * other.num)
    
    def __str__(self):
        return f"{self.num}/{self.den}"

def reduce(mat):
    n = len(mat)
    m = len(mat[0])
    d = [[[0 for y in range(m)] for x in range(n)] for i in range(n)]
    for x in range(n):
        for y in range(m):
            d[0][x][y] = mat[x][y]

    for k in range(0, n-1):
        for x in range(0, n):
            for y in range(0, m):
                d[k+1][x][y] = (d[k][x][y] * d[k][k][k] - d[k][x][k] * d[k][k][y])
                if k != 0:
                    d[k+1][x][y] //= d[k-1][k-1][k-1]


    # j >= i => a[i][j] = d[i%3][i][j] // d[i%3][i][i]
    res = [[Rational(0, 1) for _ in range(m)] for _ in range(n)]
    for x in range(n):
        for y in range(x, m):
            res[x][y] = Rational(d[x][x][y], d[x][x][x])
    
    return res

def back_subst(mat):
    n = len(mat)
    res = [None for _ in range(n)]
    for i in range(n-1, -1, -1):
        r = mat[i][n]
        for j in range(i+1, n):
            r -= res[j] * mat[i][j]
        res[i] = r / mat[i][i]
    
    return res
