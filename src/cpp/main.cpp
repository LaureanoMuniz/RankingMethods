#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cassert>

using namespace std;
using mat = vector<vector<double>>;
template<typename T>
struct Mat {
    int n, m;
	vector<vector<T>> mat;
    T &operator()(int i, int j = 0) {
        return mat[i][j];  
    }
    const T &operator()(int i, int j = 0) const {
        return mat[i][j];  
    }

	static Mat cero(int _n, int _m = 1) {
        Mat res { _n, _m, vector<vector<T>>(_n, vector<T>(_m, 0)) };
		return res;
    }
	static Mat identidad(int _n) {
        Mat res = Mat::cero(_n, _n);
        for(int i = 0; i < res.n; i++){
			res(i, i) = 1;
		}
		return res;
    }
	void operacion_2(int fila_1, int fila_2, T multiplo){
		for(int i = 0; i < m; i++){
			mat[fila_2][i] = mat[fila_2][i] + mat[fila_1][i] * multiplo;
		}
	}
	void operacion_3(int fila_1, int fila_2){
		swap(mat[fila_1], mat[fila_2]);
	}
};

struct Partido {
    string fecha;
    int puntos_i;
    int id_i;
    int puntos_j;
    int id_j;
};

struct Torneo {
    vector<string> nombres;
    vector<Partido> partidos;
    int equipos() const {
        return int(nombres.size());
    }
};

template<typename FDescr>
Torneo read_data(FDescr &file) {
    int equipos, partidos;
    file >> equipos >> partidos;
    std::map<string, int> ids_equipos;

    Torneo torneo;

    for(int i = 0; i < partidos; ++i) {
        string equipo_i;
        string equipo_j;
        Partido partido;
        file >> partido.fecha >> equipo_i >> partido.puntos_i >> equipo_j >> partido.puntos_j;
        for(auto name : {equipo_i, equipo_j}) {
            if(ids_equipos.find(name) == ids_equipos.end()) {
                int k = int(ids_equipos.size());
                torneo.nombres.push_back(name);
                ids_equipos[name] = k;
            }
        }
        partido.id_i = ids_equipos[equipo_i];
        partido.id_j = ids_equipos[equipo_j];
        torneo.partidos.push_back(partido);
    }
    return torneo;
}

pair<Mat<double>, Mat<double>> ganados_y_perdidos(const Torneo &torneo) {
    auto ganados = Mat<double>::cero(torneo.equipos());
    auto perdidos = Mat<double>::cero(torneo.equipos());
    
    for(const auto &partido : torneo.partidos) {
        int ganador, perdedor;
        if(partido.puntos_i > partido.puntos_j) {
            ganador = partido.id_i;
            perdedor = partido.id_j;
        } else if(partido.puntos_i < partido.puntos_j) {
            ganador = partido.id_j;
            perdedor = partido.id_i;
        } else assert(false);
        ganados(ganador) += 1;
        perdidos(perdedor) += 1;
    }
    return {ganados, perdidos};
}

Mat<double> sistema_CMM(const Torneo &torneo){
    auto jugados = Mat<double>::cero(torneo.equipos(), torneo.equipos());
    
    for(const auto &partido : torneo.partidos) {
        jugados(partido.id_i, partido.id_j) += 1;
        jugados(partido.id_j, partido.id_i) += 1;
    }

    auto [ganados, perdidos] = ganados_y_perdidos(torneo);

    auto sistema = Mat<double>::cero(torneo.equipos(), torneo.equipos()+1);
    for(int i = 0; i < torneo.equipos(); ++i) {
        for(int j = 0; j < torneo.equipos(); ++j) {
            if(i != j) sistema(i, j) = -jugados(i, j);
            else sistema(i, j) = 2.0 + ganados(i) + perdidos(i);
        }
        sistema(i, torneo.equipos()) = 1.0 + (ganados(i) - perdidos(i)) / 2.0;
    }
    
	return sistema;
}

template<typename T>
auto eliminacion_gaussiana(Mat<T> sistema) -> Mat<T> {
	for (int i = 0; i < sistema.n; ++i){
		assert(sistema(i,i) != 0);
		for (int j = i + 1; j < sistema.n; j++){
			T coef = sistema(j, i) / sistema(i, i);
			sistema.operacion_2(i, j, -coef);
		}
	}
	auto result = Mat<T>::cero(sistema.n);
	for (int i = sistema.n - 1; i >= 0; i--){
		T acum = 0;
		for (int j = i + 1; j < sistema.n; j++){
			acum += ( result(j) *  sistema(i, j) );
		}
		result(i) = ( sistema(i, sistema.n) - acum ) / sistema(i, i);
	}
	return result;
}

void print_rankings(const Torneo &torneo, const Mat<double> & rankings, string path){
	ofstream file(path);
	for (int i = 0; i < torneo.equipos(); ++i)
	{
		file << torneo.nombres[i] << " " << rankings(i) << endl;
	}
}

Mat<double> WP(const Torneo &torneo){

	auto result = Mat<double>::cero(torneo.equipos()); 
	auto [ganados, perdidos] = ganados_y_perdidos(torneo);
	for (int i = 0; i < torneo.equipos(); ++i)
	{
		result(i) = ganados(i)/(ganados(i)+perdidos(i));
	}
	return result;
}

void display_usage() {
    cout << R"(uso: ./tp1 algorithm source destination
algorithm:
    debe ser 0, 1 o 2
source:
    path al archivo .tsv del que se leerá la entrada
destination:
    path al archivo donde se escribirá la salida
)";
    exit(0);
}
 
int main(int argc, char **argv){
    if(argc != 4) display_usage();
	ifstream file(argv[2]);
	Torneo torneo = read_data(file);
    Mat<double> rankings;
    if (string(argv[1]) == "0"){
		rankings = eliminacion_gaussiana(sistema_CMM(torneo));	
	}
	else if(string(argv[1]) == "1"){
		rankings = WP(torneo);
	}
	else if(string(argv[1]) == "2"){
		//todo
	}else{
		cout << "Uso: metodo, inputpath, outputpath" <<  endl;
	}
	print_rankings(torneo, rankings, argv[3]);
	return 0;
}