from pathlib import Path
from random import Random
from collections import deque
from io import StringIO


class Juego:
    def __init__(self, *args):
        if len(args) == 5:
            fecha, i, pi, j, pj = args
        else:
            i, j, pi, pj = args
            fecha = "1"

        self.fecha = fecha
        self.i = i
        self.j = j
        self.pi = pi
        self.pj = pj


class Torneo:
    def __init__(self, n, juegos):
        self.n = n
        self.m = len(juegos)
        self.juegos = juegos

    def __str__(self):
        stream = StringIO()
        stream.write(f"{self.n} {self.m}\n")
        for juego in self.juegos:
            stream.write(f"{juego.fecha} {juego.i} {juego.pi} {juego.j} {juego.pj}\n")

        return stream.getvalue()

    def from_file(filepath: Path):
        with filepath.open() as f:
            lines = f.readlines()
            n, _ = lines[0].split()
            juegos = list(map(lambda line: Juego(*line.split()), lines[1:]))
            return Torneo(n, juegos)


# Esperanza de aristas = (1/k * ri + (1 - 1/k) * ro) * (n choose 2)
def clusters(rng: Random, n, k, r, ro):
    ri = k*r - (k - 1) * ro
    cluster = [0] * n
    for i in range(n):
        cluster[i] = rng.randrange(0, k)

    juegos = []
    for i in range(n):
        for j in range(i):
            if cluster[i] == cluster[j]:
                proba = ri
            else:
                proba = ro
            if rng.random() < proba:
                juegos.append(Juego(i, j, 1, 0))

    return Torneo(n, juegos)


def todos_contra_todos(rng: Random, n, r):
    return clusters(rng, n, 1, r, 0)


def torneo(rng: Random, n):
    equipos = deque(range(n))
    juegos = []
    while len(equipos) > 1:
        a = equipos.popleft()
        b = equipos.popleft()
        if rng.randint(0, 1) < 1:
            a, b = b, a
        juegos.append(Juego(a, b, 1, 0))
        equipos.append(a)

    return Torneo(n, juegos)


def diagonal(n):
    juegos = [Juego(i, i+1, 0, 1) for i in range(n-1)]
    return Torneo(n, juegos)
