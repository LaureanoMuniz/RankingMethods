import math


class Rational():
    def __init__(self, num, den):
        if den < 0:
            num = -num
            den = abs(den)
        self.num = num
        self.den = den

    def __add__(self, other):
        return Rational(
            self.num * other.den + self.den * other.num,
            self.den * other.den)

    def __neg__(self):
        return Rational(-self.num, self.den)

    def __sub__(self, other):
        return self + (-other)

    def __mul__(self, other):
        return Rational(self.num * other.num, self.den * other.den)

    def __truediv__(self, other):
        return Rational(self.num * other.den, self.den * other.num)

    def __str__(self):
        return f"{self.num}/{self.den} = {self.num/self.den}"

    def __abs__(self):
        return Rational(abs(self.num), self.den)

    def __gt__(self, other):
        return self.num * other.den > other.num * self.den


def reduce(mat):
    n = len(mat)
    m = len(mat[0])
    d = [[[0 for y in range(m)] for x in range(n)] for i in range(3)]
    for x in range(n):
        for y in range(m):
            d[0][x][y] = mat[x][y]

    res = [[Rational(0, 1) for _ in range(m)] for _ in range(n)]
    res[0][0] = Rational(mat[0][0], 1)
    for k in range(0, n-1):
        # print(f"k = {k}")
        k1 = (k+1)%3
        k2 = (k+2)%3
        kk = k%3
        # print(d[k%3][k][k])
        # print(f"k = {k}")
        # for y in range(k, m):
        #     res[k][y] = Rational(d[k][k][y], d[k][k][k])
        for x in range(k, n):
            # print(f"x = {x}")
            for y in range(k, m):
                # print(f"y = {y}")
                d[k1][x][y] = (d[kk][x][y] * d[kk][k][k] -
                                d[kk][x][k] * d[kk][k][y])
                if k != 0:
                    d[k1][x][y] //= d[k2][k-1][k-1]
        x = k+1
        for y in range(x, m):
            # print(f"y = {y}")
            res[x][y] = Rational(d[x%3][x][y], d[x%3][x][x])

    # j >= i => a[i][j] = d[i%3][i][j] // d[i%3][i][i]
    # res = [[Rational(0, 1) for _ in range(m)] for _ in range(n)]
    # for x in range(n):
        # for y in range(x, m):
            # res[x][y] = Rational(d[x][x][y], d[x][x][x])

    return res


def back_subst(mat):
    n = len(mat)
    res = [None for _ in range(n)]
    for i in range(n-1, -1, -1):
        # print(f"i = {i}")
        r = mat[i][n]
        for j in range(i+1, n):
            r -= res[j] * mat[i][j]
        assert mat[i][i].num != 0
        a = r / mat[i][i]
        # print(a)
        g = math.gcd(a.num, a.den)
        a.num //= g
        a.den //= g
        res[i] = a

    return res
