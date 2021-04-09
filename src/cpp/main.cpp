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

using namespace std;

struct Config {
    bool float_output_exact = false;
} config;

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
    Mat<T> extender(Mat <T> A){
        assert(A.m==1 && A.n == n);
        auto res = Mat<T>::cero(n, m + 1);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                res(i, j) = mat[i][j];
            }
        }
        for(int i = 0; i < m; i++){
            res(i, m) = A(i); 
        }
        return res;
    }
    Mat<T> transpuesta(){
        auto res = Mat<T>::cero(m, n);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                res(j, i) = mat[i][j];
            }
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

template<typename T>
struct Mat_Rala {
    int n, m;
	vector<vector<pair<int,T>>> mat;
    T zero = 0;
    T &operator()(int i, int j = 0) {
        int l = 0, h = mat[i].size();
        if(h == 0){
            mat[i].push_back({j, 0});
        }
        while(h - l > 1){
            int mid = (h + l) / 2;
            if(mat[i][mid].first > j){
                h = mid;
            }
            else{
                l = mid;
            }
        }
        if(mat[i][l].first != j){
            if(l==0 && mat[i][l].first > j){
                mat[i].insert(mat[i].begin(), {j, 0});
            }
            else{
                mat[i].insert(mat[i].begin() + l + 1, {j, 0});
                l++;
            }
        }
        return mat[i][l].second;
    }
    const T &operator()(int i, int j = 0) const {
        int l = 0, h = mat[i].size();
        if(h == 0){
            return zero;
        }
        while(h - l > 1){
            int mid = (h + l) / 2;
            if(mat[i][mid].first > j){
                h = mid;
            }
            else{
                l = mid;
            }
        }
        if(mat[i][l].first == j){
            return mat[i][l].second;
        }
        else{
            return zero;
        }
    }   
	static Mat_Rala cero(int _n, int _m = 1) {
        Mat_Rala res { _n, _m, vector<vector<pair<int,T>>>(_n) };
		return res;
    }
	static Mat_Rala identidad(int _n) {
        Mat_Rala res = Mat_Rala::cero(_n, _n);
        for(int i = 0; i < res.n; i++){
			res(i, i) = 1;
		}
		return res;
    }
    Mat_Rala<T> extender(Mat_Rala <T> A){
        assert(A.m==1 && A.n == n);
        auto res = Mat_Rala<T>::cero(n, m + 1);
        for(int i = 0; i < n; i++){
            for(auto x: mat[i]){
                res(i, x.first) = x.second;
            }
        }
        for(int i = 0; i < m; i++){
            res(i, m) = A(i); 
        }
        return res;
    }
    Mat_Rala<T> transpuesta(){
        auto res = Mat_Rala<T>::cero(m, n);
        for(int i = 0; i < n; i++){
            for(auto x: mat[i]){
                res(x.first, i) = x.second;
            }
        }
        return res;
    }
	void operacion_2(int fila_1, int fila_2, T multiplo){
        for(auto x : mat[fila_1]){
            int j = x.first;
            int l = 0, h = mat[fila_2].size();
            if(h == 0){
                mat[fila_2].push_back({j, 0});
            }
            while(h - l > 1){
                int mid = (h + l) / 2;
                if(mat[fila_2][mid].first > j){
                    h = mid;
                }
                else{
                    l = mid;
                }
            }
            if(mat[fila_2][l].first != j){
                if(l==0 && mat[fila_2][l].first > j){
                    mat[fila_2].insert(mat[fila_2].begin(), {j, 0});
                }
                else{
                    mat[fila_2].insert(mat[fila_2].begin() + l + 1, {j, 0});
                    l++;
                }
            }
            mat[fila_2][l].second = mat[fila_2][l].second + x.second * multiplo;
            if(mat[fila_2][l].second == 0){
                mat[fila_2].erase(mat[fila_2].begin()+l);
            }
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

template<typename T>
auto ganados_y_perdidos(const Torneo &torneo) -> pair<Mat<T>, Mat<T>> {
    auto ganados = Mat<T>::cero(torneo.equipos());
    auto perdidos = Mat<T>::cero(torneo.equipos());
    
    for(const auto &partido : torneo.partidos) {
        if(partido.puntos_i == partido.puntos_j) {
            ganados(partido.id_i) +=  0.5;
            perdidos(partido.id_i) += 0.5;
            ganados(partido.id_j) +=  0.5;
            perdidos(partido.id_j) += 0.5;
        } else {
            int ganador, perdedor;
            if(partido.puntos_i > partido.puntos_j) {
                ganador = partido.id_i;
                perdedor = partido.id_j;
            } else {
                ganador = partido.id_j;
                perdedor = partido.id_i;
            }
            ganados(ganador) += 1;
            perdidos(perdedor) += 1;
        }
    }
    return {ganados, perdidos};
}


template<typename T>
auto sistema_CMM(const Torneo &torneo) -> pair< Mat<T>, Mat<T> > {
    auto jugados = Mat<T>::cero(torneo.equipos(), torneo.equipos());
    for(const auto &partido : torneo.partidos) {
        jugados(partido.id_i, partido.id_j) += 1;
        jugados(partido.id_j, partido.id_i) += 1;
    }
   
    auto [ganados, perdidos] = ganados_y_perdidos<T>(torneo);

    auto sistema = Mat<T>::cero(torneo.equipos(), torneo.equipos());
    auto b = Mat<T>::cero(torneo.equipos());
    for(int i = 0; i < torneo.equipos(); ++i) {
        for(int j = 0; j < torneo.equipos(); ++j) {
            if(i != j) sistema(i, j) = -jugados(i, j);
            else sistema(i, j) = 2.0 + ganados(i) + perdidos(i);
        }
        b(i) = 1.0 + (ganados(i) - perdidos(i)) / 2.0;
    }
    
    
	return {sistema, b};
}

template<typename T>
auto backwards_substitution(Mat<T> &sistema) -> Mat<T>{
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

template<typename T>
auto forward_substitution(Mat<T> &sistema) -> Mat<T>{
    auto result = Mat<T>::cero(sistema.n);
	for (int i = 0; i < sistema.n; i++){
		T acum = 0;
		for (int j = 0; j < i; j++){
			acum += ( result(j) *  sistema(i, j) );
		}
		result(i) = ( sistema(i, sistema.n) - acum ) / sistema(i, i);
	}
    return result;
}

template<typename T>
auto eliminacion_gaussiana(Mat<T> A, Mat<T> b) -> Mat<T> {
    auto sistema = A.extender(b);
	for (int i = 0; i < sistema.n; ++i){
		assert(sistema(i,i) != 0);
		for (int j = i + 1; j < sistema.n; j++){
			T coef = sistema(j, i) / sistema(i, i);
			sistema.operacion_2(i, j, -coef);
		}
	}
	return backwards_substitution(sistema);
}

template<typename T> 
auto cholesky(Mat<T> A, Mat<T> b) -> Mat<T> {
    auto L = Mat<T>::cero(A.n, A.n);
    for(int i = 0; i < A.n; i++){
        for(int j = 0; j <= i; j++){
            T acum = 0;
            if ( i == j ){ // En la diagonal
                for(int k = 0; k < i;k++){
                    acum += pow(L(i, k), 2);
                }
                L(i, i) = sqrt(A(i, i) - acum);
            } else {
                for(int k = 0; k < j; k++){
                    acum += L(i, k) * L(j, k);
                }
                L(i, j) = (A(i, j) - acum) / L(j, j);
            }
                   
        }
            
    }
    auto sistema = L.extender(b); // L extendida con b al final.
    auto y = forward_substitution(sistema); // Forward substitution L.y = b.
    auto Lt = L.transpuesta();
    sistema = Lt.extender(y);  // Lt extendida con y al final.
    auto x = backwards_substitution(sistema); // Backwards substitution L^t x = y
    return x;
}

template<typename Of>
void print_rankings(const Torneo &torneo, const Mat<double> & rankings, Of &file) {
    vector<pair<int, double>> results;
	for (int i = 0; i < torneo.equipos(); ++i) {
        results.emplace_back(stoi(torneo.nombres[i]), rankings(i));
	}
    sort(results.begin(), results.end());
	for(auto [_, rank] : results) {
        file << rank << endl;
	}
}

Mat<double> WP(const Torneo &torneo){

	auto result = Mat<double>::cero(torneo.equipos()); 
	auto [ganados, perdidos] = ganados_y_perdidos<double>(torneo);
	for (int i = 0; i < torneo.equipos(); ++i)
	{
		result(i) = ganados(i)/(ganados(i)+perdidos(i));
	}
	return result;
}

double elo_expectedScore(const double ratingDiff){
    double exponente = ratingDiff/(400.0);
    return 1 / (1 - pow(10,exponente));
}

Mat<double> elo_Ratings(const Torneo &torneo, const double K = 32){
    Mat<double> ratings = Mat<double>::cero(torneo.equipos()); 
    for (int i = 0; i < torneo.equipos(); i++){
        ratings(i) = 1000.0;
    }
    for (auto partido_actual: torneo.partidos){
        //numeros que hacen falta
        double scoreI,scoreJ = 0.5;
        if(partido_actual.puntos_i > partido_actual.puntos_j){scoreI = 0; scoreJ = 1;}
        else if (partido_actual.puntos_i < partido_actual.puntos_j){scoreI = 1; scoreJ = 0;}
        double ratingDiff_J = ratings(partido_actual.id_i) - ratings(partido_actual.id_j);
        double ratingDiff_I = - ratingDiff_J;
        double expected_J = elo_expectedScore(ratingDiff_J);
        double expected_I = elo_expectedScore(ratingDiff_I);

        //se actualizan los ratings 
        ratings(partido_actual.id_i) += (scoreI - expected_I) * K;
        ratings(partido_actual.id_j) += (scoreJ - expected_J) * K; 
    }
    return ratings;
}

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
 
int main(int argc, char **argv) {
    if(argc < 4) display_usage();

    for(int i = 4; i < argc; ++i) {
        string s = string(argv[i]);
        if(s == "float_output_exact") config.float_output_exact = true;
    }

	ifstream file(argv[1]);
	Torneo torneo = read_data(file);
    Mat<double> rankings;
    if (string(argv[3]) == "0") {
        auto sistema = sistema_CMM<double>(torneo);
		rankings = eliminacion_gaussiana(sistema.first, sistema.second);	
	}
	else if(string(argv[3]) == "1"){
		rankings = WP(torneo);
	}
	else if(string(argv[3]) == "2"){
		rankings = elo_Ratings(torneo);
	}
    else if(string(argv[3]) == "3"){
        auto sistema = sistema_CMM<double>(torneo);
        rankings = cholesky(sistema.first, sistema.second);
    }
    else display_usage();

    if(string(argv[2]) == "-") {
        ofstream output(argv[2]);
	    print_rankings(torneo, rankings, output);
    } else {
	    print_rankings(torneo, rankings, std::cout);
    }
	return 0;
}