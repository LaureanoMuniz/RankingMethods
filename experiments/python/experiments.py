import math
from random import Random
from experiments.python.generadores import Torneo, clusters
from experiments.python.rational import Rational, reduce, back_subst
import enum
from pathlib import Path
import subprocess

import matplotlib.pyplot as plt

from tqdm import tqdm


class Args(enum.Enum):
    DISPLAY_MATRIX = "display_matrix"
    EXACT_OUTPUT = "float_output_exact"
    RALA = "rala"
    TIME = "time"
    INTERNAL_ID = "internal_id"
    SHOW_ID = "show_id"
    SIZE = "size"

    CMM = "0"
    WP = "1"
    ELO = "2"
    CHOLESKY = "3"


class Ranking:
    def __init__(self, ranking: dict):
        self.ranking = ranking

    def distance(self, o):
        total = 0.0
        print(list(self.ranking.keys())[:10])
        print(list(o.ranking.keys())[:10])
        for k in self.ranking.keys():
            if k in o.ranking:
                total += math.log(self.ranking[k]) - math.log(o.ranking[k])
        return total

    def from_file(file):
        with file.open() as f:
            lines = f.readlines()
            ranking = dict()
            for line in lines:
                _, pos, id, _ = line.split(',')
                ranking[id] = int(pos)
            return Ranking(ranking)


class Corrida:
    def __init__(self, torneo, *args):
        if type(torneo) is not Torneo:
            torneo = Torneo.from_file(torneo)

        if Args.SIZE in args:
            self.n, self.m = torneo.n, torneo.m

        command = [
            './tp',
            '-',
            '-',
            *map(lambda x: x.value, args)]
        result = subprocess.run(
            command,
            stdout=subprocess.PIPE,
            text=True,
            input=str(torneo))
        # print(f"torneo: Torneo")
        # print(" ".join(command))
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
        elif Args.SHOW_ID in args:
            # print("values:")
            # print(result.stdout)
            rating = dict()
            for i in range(len(values) // 2):
                rating[values[2*i]] = float(values[2*i+1])
            ranking = list(rating.keys())
            ranking.sort(key=lambda x: rating[x])
            self.ranking = Ranking({
                ranking[i]: i+1 for i in range(len(ranking))})
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


def tiempo_random():
    fig, ax = plt.subplots()
    rng = Random(1)
    k = 10
    xs = []
    ys = []
    cs = []
    for k in tqdm([-1, 1, 100]):
        for n in tqdm(range(500, 1001, 100)):
            q = k
            if k == -1:
                q = 1
            torneo = clusters(rng, n, q, 1/200, 0.0)
            if k == -1:
                time = Corrida(torneo, Args.CMM, Args.TIME).elapsed
            else:
                time = Corrida(torneo, Args.CMM, Args.RALA, Args.TIME).elapsed
            xs.append(n)
            ys.append(math.log(time))
            cs.append(k)
    mp = ax.scatter(xs, ys, c=cs, cmap='viridis')

    fig.colorbar(mp)

    plt.show()


def distance(filepath):
    fig, ax = plt.subplots()
    posta = Ranking.from_file(
        Path() / 'tests' / 'Tests_Propios' / 'Tennis_Ranking_2021.csv')

    xs = []
    ys = []

    for algo in [Args.CMM, Args.ELO, Args.WP]:
        rankings = Corrida(filepath, algo, Args.SHOW_ID).ranking
        xs.append(algo.value)
        ys.append(rankings.distance(posta))
        print(ys[-1])

    ax.bar(xs, ys)
    plt.show()


def experimentar():
    """tiempo([
        'test-prob-1.in',
        'test-prob-2.in',
        'test1.in',
        'test2.in',
        'test_completos/test_completo_10_1.in',
        'test_completos/test_completo_100_4.in',
        'test_completos/test_completo_1000_8.in',
    ])"""
    # precision(['test_completos/test_completo_100_4.in'])
    # tiempo_random()
    distance(Path() / 'tests' / 'Tests_Propios' / 'Tenis_2020_21.dat')
    # precision(['test-prob-1.in'])
