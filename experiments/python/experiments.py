from experiments.python.rational import Rational, reduce, back_subst
import enum
from pathlib import Path
import subprocess

import matplotlib.pyplot as plt
import numpy as py

import math

class Args(enum.Enum):
    DISPLAY_MATRIX = "display_matrix"
    EXACT_OUTPUT = "float_output_exact"
    RALA = "rala"
    TIME = "time"
    INTERNAL_ID = "internal_id"
    SIZE = "size"

    CMM = "0"
    WP = "1"
    ELO = "2"
    CHOLESKY = "3"


class Corrida:
    def __init__(self, filepath, *args):
        path = Path() / 'tests' / filepath
        if Args.SIZE in args:
            with path.open() as f:
                self.n, self.m = map(int, f.readline().split(' '))
            return

        result = subprocess.run([
            './tp',
            path,
            '-',
            *map(lambda x: x.value, args)],
            stdout=subprocess.PIPE,
            text=True)

        values = result.stdout.split()
        # print(result.stdout)
        if Args.EXACT_OUTPUT in args:
            self.ratings = []
            for i in range(len(values) // 2):
                self.ratings.append(Rational(
                    int(values[2*i]),
                    2**int(values[2*i+1])))
        elif Args.DISPLAY_MATRIX in args:
            n = int(values[0])
            self.mat = [[0 for i in range(n+1)] for i in range(n)]
            for i in range(n):
                for j in range(n):
                    self.mat[i][j] = int(values[1 + n*i + j])
            for i in range(n):
                self.mat[i][n] = int(values[1 + n*n + i])
        elif Args.TIME in args:
            self.elapsed = int(values[0]) * 1e-6
        else:
            self.ratings = list(map(lambda x: float(x), values))


def error(values, exact):
    diff = Rational(0, 1)
    for i in range(0, len(exact)):
        ex = exact[i]*Rational(1, 2)
        # print(f"{ex}, {values[i]}")
        # print(f"{exact[i].num} {exact[i].den} {values[i].num} {values[i].den}")
        r = abs(ex - values[i])
        # print(f" r = {r.num} {r.den}")

        if r > diff or True:
            diff = r

    # print(f" = {diff.num} / {diff.den}")
    return diff.num / diff.den


def precision(filepaths):
    for f in filepaths:
        cmm = Corrida(f, Args.CMM, Args.EXACT_OUTPUT, Args.INTERNAL_ID)
        cova = Corrida(f, Args.CHOLESKY, Args.EXACT_OUTPUT, Args.INTERNAL_ID)
        exact = Corrida(f, Args.CMM, Args.DISPLAY_MATRIX)
        mat = reduce(exact.mat)
        ex = back_subst(mat)
        print(error(cmm.ratings, ex))
        print(error(cova.ratings, ex))
    pass


def tiempo(filepaths):
    fig, ax = plt.subplots()
    for f in filepaths:
        n = Corrida(f, Args.SIZE).n
        cmm = Corrida(f, Args.CMM, Args.TIME).elapsed
        cho = Corrida(f, Args.CHOLESKY, Args.TIME).elapsed
        ax.scatter(n, cmm, c="tab:blue")
        ax.scatter(n, cho, c="tab:orange")

    plt.show()


def experimentar():
    tiempo([
        'test-prob-1.in',
        'test-prob-2.in',
        'test1.in',
        'test2.in',
        'test_completos/test_completo_10_1.in',
        'test_completos/test_completo_100_4.in',
        'test_completos/test_completo_1000_8.in',
    ])
    # precision(['test_completos/test_completo_100_4.in'])
    # precision(['test-prob-1.in'])
