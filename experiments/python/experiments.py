import math
from random import Random
from experiments.python.generadores import Juego, Torneo, clusters, diagonal, torneo
from experiments.python.rational import Rational, reduce, back_subst
import enum
from pathlib import Path
import subprocess
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt

from tqdm import tqdm

import numpy as np


class Args(enum.Enum):
    DISPLAY_MATRIX = "display_matrix"
    EXACT_OUTPUT = "float_output_exact"
    RALA = "rala"
    TIME = "time"
    INTERNAL_ID = "internal_id"
    SHOW_ID = "show_id"
    SIZE = "size"
    FLOAT = "float"

    CMM = "0"
    WP = "1"
    ELO = "2"
    CHOLESKY = "3"


class Ranking:
    def __init__(self, ranking: dict):
        self.ranking = ranking

    def distances(self, o):
        distances = []
        for k in self.ranking.keys():
            if k in o.ranking:
                distances.append(
                    math.log(self.ranking[k]) - math.log(o.ranking[k]))
        return distances

    def distance(self, o):
        total = 0
        dists = self.distances(o)
        for d in dists:
            total += abs(d)
        return math.exp(total / len(dists)) - 1

    def from_file(file):
        with file.open() as f:
            lines = f.readlines()
            ranking = dict()
            for line in lines:
                _, pos, id, _ = line.split(',')
                ranking[id] = int(pos)
            return Ranking(ranking)

    def filt(self, equipos):
        by_ranking = []
        for e in self.ranking.keys():
            if e in equipos:
                while len(by_ranking) <= self.ranking[e]:
                    by_ranking.append([])
                by_ranking[self.ranking[e]-1].append(e)
        last = 0
        nuevo = dict()
        for es in by_ranking:
            if len(es) != 0:
                last += 1
                for e in es:
                    nuevo[e] = last

        return Ranking(nuevo)


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
            ranking.sort(key=lambda x: -rating[x])
            self.ranking = Ranking({
                ranking[i]: i+1 for i in range(len(ranking))})
            self.ratings = rating
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
    print(f"diff = {diff.num}")
    return diff.num / diff.den


def precision():
    filepaths = [
        Path('tests/Tests_Propios/Futbol_2014.dat'),
        Path('tests/Tests_Propios/Futbol_2010.dat'),
        Path('tests/Tests_Propios/NBA_2020.dat'),
        Path('tests/Tests_Propios/NBA_2019.dat'),
    ]
    xs = ["GE+double", "CHOLESKY+double", "GE+float", "CHOLESKY+float"]
    cs = ["tab:orange", "tab:blue", "tab:green", "tab:purple"]
    labs = ["Futbol 2014", "Furbol 2010", "NBA 2020", "NBA 2019"]
    fig, axs = plt.subplots(len(filepaths), sharex=True)
    fig.suptitle('Precisión')
    for f, ax, lab in zip(filepaths, axs, labs):
        cmm = Corrida(f, Args.CMM, Args.EXACT_OUTPUT, Args.INTERNAL_ID)
        cho = Corrida(f, Args.CHOLESKY, Args.EXACT_OUTPUT, Args.INTERNAL_ID)
        cmmf = Corrida(f, Args.CMM, Args.EXACT_OUTPUT, Args.INTERNAL_ID, Args.FLOAT)
        chof = Corrida(f, Args.CHOLESKY, Args.EXACT_OUTPUT, Args.INTERNAL_ID, Args.FLOAT)

        exact = Corrida(f, Args.CMM, Args.DISPLAY_MATRIX)
        mat = reduce(exact.mat)
        ex = back_subst(mat)

        ys = [
            error(cmm.ratings, ex),
            error(cho.ratings, ex),
            error(cmmf.ratings, ex),
            error(chof.ratings, ex),
        ]

        for e in ys:
            print(e)

        ax.set_xscale('log')
        h = ax.barh(np.arange(4), ys, color=cs)
        # ax.set_yticks(np.arange(4))
        # ax.set_yticklabels(xs)
        ax.set_yticks(ticks=[])
        ax.set_xlim(left=1e-17, right=1)
        ax.set_ylabel(lab)
        # ax.legend(["GE+double", "CHOLESKY+double", "GE+float", "CHOLESKY+float"])

    fig.legend(h, xs, loc="upper right")
    plt.xlabel("error calculado con la norma infinito (sin unidad) (escala log)")
    plt.savefig('experiments/results/precision.png')


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
    fig, axs = plt.subplots(1, 3, sharey=True)
    rng = Random(1)

    fig.suptitle('Tiempo de Ejecución en Distintas Instancias')

    axs[0].set_ylabel('tiempo de ejecución (s) (escala log)')

    for ax, to in tqdm(zip(axs, [
            lambda n: clusters(rng, n, 1, 1/200, 1/2000),
            lambda n: clusters(rng, n, 100, 1/200, 1/2000),
            lambda n: torneo(rng, n)])):
        ax.set_xlabel('cantidad de equipos')
        xs = []
        ys = []
        cs = []
        for n in tqdm(range(500, 1001, 100)):
            for lala in range(10):
                for args, col in [((Args.CHOLESKY,), "tab:orange"), ((Args.CMM,), "tab:blue"), ((Args.CMM, Args.RALA), "tab:green")]:
                    tor = to(n)
                    time = Corrida(tor, *args, Args.TIME).elapsed
                    xs.append(n)
                    ys.append(time)
                    cs.append(col)
        # ax.scale_y('log')
        mp = ax.scatter(xs, ys, c=cs)


    # fig.colorbar(mp)
    plt.yscale('log')
    # fig.ylabel('')
    cho = mpatches.Patch(color='tab:orange', label='cholesky')
    eg = mpatches.Patch(color='tab:blue', label='EG')
    egrala = mpatches.Patch(color='tab:green', label='EG+Rala')
    fig.legend(handles=[cho, eg, egrala])

    fig.set_size_inches(fig.get_size_inches()[0]*2, fig.get_size_inches()[1])

    plt.savefig('experiments/results/tiempo.png')


def diag():
    fig, ax = plt.subplots()
    xs = []
    ys = []
    cs = []

    for n in tqdm(range(100, 1001, 100)):
        torneo = diagonal(n)
        rala = Corrida(torneo, Args.CMM, Args.TIME, Args.RALA).elapsed
        normal = Corrida(torneo, Args.CMM, Args.TIME).elapsed
        xs.append(n)
        xs.append(n)
        ys.append(math.log(rala))
        ys.append(math.log(normal))
        cs.append(0)
        cs.append(1)
    mp = ax.scatter(xs, ys, c=cs, cmap='viridis')

    fig.colorbar(mp)

    plt.show()


def distance(filepath):
    fig, ax = plt.subplots()
    posta = Ranking.from_file(
        Path() / 'tests' / 'Tests_Propios' / 'Tennis_Ranking_2021.csv')

    xs = []
    cs = []
    fig.suptitle('Distancias al Ranking Oficial')
    for algo, name in [(Args.CMM, "CMM"), (Args.ELO, "ELO"), (Args.WP, "WP")]:
        rankings = Corrida(filepath, algo, Args.SHOW_ID).ranking
        rankings = rankings.filt(posta.ranking.keys())
        dis = rankings.distance(posta)
        # for d in rankings.distances(posta):
        # cs.append(algo.value)
        # ys.append(d)
        cs.append(dis)
        xs.append(name)

        # ax.hist(ys, bins=20, histtype='step')
    ax.bar(xs, cs)
    plt.ylabel('distancia eta al ranking oficial (sin unidad)')
    plt.savefig('experiments/results/distance.png')


def empate(file):
    tor = Torneo.from_file(file)
    ratings = Corrida(tor, Args.CMM, Args.SHOW_ID).ratings
    jugadores = list(ratings.keys())

    fig, ax = plt.subplots()
    fig.suptitle('Efecto del Empate')
    rng = Random(1)
    xs = []
    ys = []
    for sdjfhkaf in range(100):
        a = jugadores[rng.randint(0, len(jugadores)-1)]
        while True:
            b = jugadores[rng.randint(0, len(jugadores)-1)]
            if a != b:
                break

        juegos = tor.juegos.copy()
        juegos.append(Juego(a, b, 0, 0))

        new = Corrida(Torneo(tor.n, juegos), Args.CMM, Args.SHOW_ID).ratings

        xs.append(ratings[b]-ratings[a])
        ys.append(new[a]-ratings[a])

    ax.scatter(xs, ys)
    plt.ylabel('variación del rating (sin unidad)')
    plt.xlabel('diferencia entre los rankings de los equipos (sin unidad)')
    plt.axhline(0, color='black')
    plt.axvline(0, color='black')
    plt.savefig('experiments/results/empate.png')


def diff(filepaths):

    def ranking(file, algo):
        return Corrida(file, algo, Args.SHOW_ID).ranking

    xs = [0]*6
    ys = [0]*6
    cs = [0]*6
    for i, name, col, file in zip([0, 1], ['', ' '], ['tab:orange', 'tab:blue'], filepaths):

        rankings = {algo: ranking(file, algo) for algo in [Args.CMM, Args.ELO, Args.WP]}

        xs[i] = f'CMM-ELO{name}'
        ys[i] = rankings[Args.CMM].distance(rankings[Args.ELO])

        xs[i+2] = f'ELO-WP{name}'
        ys[i+2] = rankings[Args.ELO].distance(rankings[Args.WP])

        xs[i+4] = f'WP-CMM{name}'
        ys[i+4] = rankings[Args.WP].distance(rankings[Args.CMM])
        # ax.hist(ys, bins=20, histtype='step')

        cs[i] = col
        cs[i+2] = col
        cs[i+4] = col

    fig, ax = plt.subplots()
    fig.suptitle('Distancias Entre los Rankings')

    ax.bar(xs, ys, color=cs)
    plt.ylabel('distancia eta (sin unidad)')
    premier = mpatches.Patch(color='tab:orange', label='premier league')
    nba = mpatches.Patch(color='tab:blue', label='nba')
    fig.legend(handles=[premier, nba])
    plt.savefig(f'experiments/results/diffs.png')


def logistic(f: float):
    return math.tanh(f)/2 + .5


def estrategia(filepath):
    rng = Random(1)

    torneo = Torneo.from_file(filepath)
    c = Corrida(torneo, Args.CHOLESKY, Args.SHOW_ID)
    ratings = c.ratings
    rankings = c.ranking.ranking
    equipos = list(ratings.keys())

    def esperanza(yo, x):
        return -logistic(ratings[yo] - ratings[x]) * ratings[x]

    methods = [lambda yo, x: -ratings[x], lambda yo, x: ratings[x], esperanza]

    eqs = rng.shuffle(equipos)
    eqs = equipos

    fig, ax = plt.subplots()

    ys = []
    xs = []
    cs = []

    for method, col in zip(methods, ["tab:orange", "tab:green", "tab:blue"]):
        arr = np.zeros(len(eqs))
        for eq, i in zip(eqs, range(len(eqs))):
            orig = rankings[eq]
            equipos.sort(key=lambda x: method(eq, x))
            elegidos = equipos[:10]
            juegos = torneo.juegos.copy()
            for equipo in elegidos:
                if rng.random() < logistic(ratings[equipo] - ratings[eq]):
                    juegos.append(Juego(equipo, eq, 1, 0))
                else:
                    juegos.append(Juego(equipo, eq, 0, 1))
            modificado = Torneo(torneo.n, juegos)

            rank = Corrida(modificado, Args.CHOLESKY, Args.SHOW_ID).ranking.ranking[eq]
            diff = math.log(orig) - math.log(rank)
            ys.append(diff)
            arr[i] = diff
            xs.append(orig)

            cs.append(col)
        print(f"{arr.mean()} {arr.var()}")

    plt.ylabel('mejora en el logaritmo del ranking (sin unidad)')
    plt.xlabel('ranking inical (sin unidad)')
    ax.scatter(xs, ys, c=cs, s=2)

    fig.legend(handles=[
        mpatches.Patch(color='tab:orange', label='estrategia #2'),
        mpatches.Patch(color='tab:green', label='estrategia #1'),
        mpatches.Patch(color='tab:blue', label='estrategia #3')
    ])
    fig.suptitle('Estrategias Para Aumentar el Rating')
    plt.savefig('experiments/results/estrategia.png')


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
    # precision([Path('tests/test_completos/test_completo_100_4.in')])
    # precision()
    """precision([
        Path('tests/test_completos/test_completo_100_4.in'),
        Path('tests/test_completos/test_completo_100_4.in')
        ])"""
    # tiempo_random()
    # estrategia()
    # diag()
    '''diff([
        Path() / 'tests' / 'Tests_Propios' / 'premierleague.dat',
        Path() / 'tests' / 'Tests_Propios' / 'NBA_2020.dat'
    ])'''
    distance(Path() / 'tests' / 'Tests_Propios' / 'Tenis_2020_21.dat')
    # empate(Path() / 'tests' / 'Tests_Propios' / 'NBA_2020.dat')
    # estrategia(Path() / 'tests' / 'Tests_Propios' / 'Tenis_2020_21.dat')
    # precision(['test-prob-1.in'])
