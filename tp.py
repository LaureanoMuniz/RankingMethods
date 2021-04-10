import sys
from pathlib import Path
import subprocess

from experiments.python.experiments import experimentar


def compile():
    subprocess.call([
        'g++',
        '--std=c++17',
        '-O2',
        Path() / 'src' / 'cpp' / 'main.cpp',
        '-o', Path() / 'tp'])


def main():
    assert len(sys.argv) == 2
    command = sys.argv[1]
    if command == 'compile':
        compile()
    elif command == 'experimentos':
        compile()
        experimentar()


if __name__ == "__main__":
    main()
