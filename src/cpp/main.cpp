#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <algorithm>
#include <math.h>
#include <chrono>
using namespace std;

using ld = double;

struct Config {
    bool float_output_exact = false;
    bool display_matrix = false;
    bool time = false;
    bool rala = false;
    bool internal_id = false;
    string input;
    string output;
    string algo;
} config;

void display_usage() {
    cout << R"(uso: ./tp1 source destination algorithm
algorithm:
    debe ser 0, 1 , 2 o 3
source:
    path al archivo .tsv del que se leerá la entrada
destination:
    path al archivo donde se escribirá la salida
)";
    exit(0);
}

// No me asesinen
namespace Normal {
    #include "mat.h"
    #include "algos.h"
};

namespace Rala {
    #include "mat_rala.h"
    #include "algos.h"
};



int main(int argc, char **argv) {

    if(argc < 4) display_usage();

    config.input = string(argv[1]);
    config.output = string(argv[2]);
    config.algo = string(argv[3]);

    for(int i = 4; i < argc; ++i) {
        string s = string(argv[i]);
        if(s == "float_output_exact") config.float_output_exact = true;
        else if(s == "display_matrix") config.display_matrix = true;
        else if(s == "time") config.time = true;
        else if(s == "rala") config.rala = true;
        else if(s == "internal_id") config.internal_id = true;
    }

    if(config.rala) Rala::correr();
    else Normal::correr();
    return 0;
}
