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

struct Config {
    bool float_output_exact = false;
    bool display_matrix = false;
    bool time = false;
    bool rala = false;
    bool internal_id = false;
    bool show_id = false;
    bool single = false;
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
namespace NormalDouble {
    #define ld double
    #include "mat.h"
    #include "algos.h"
};

namespace RalaDouble {
    #define ld double
    #include "mat_rala.h"
    #include "algos.h"
};

namespace NormalFloat {
    #define ld float
    #include "mat.h"
    #include "algos.h"
};

namespace RalaFloat {
    #define ld float
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
        else if(s == "show_id") config.show_id = true;
        else if(s == "float") config.single = true;
    }

    if(config.rala) {
        if(config.single) RalaFloat::correr();
        else RalaDouble::correr();
    } else {
        if(config.single) NormalFloat::correr();
        else NormalDouble::correr();
    }
    return 0;
}
