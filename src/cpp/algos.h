#include <string>
#include <vector>
using namespace std;

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

pair<Mat, Mat> ganados_y_perdidos(const Torneo &torneo) {
    auto ganados = Mat::cero(torneo.equipos());
    auto perdidos = Mat::cero(torneo.equipos());
    
    for(const auto &partido : torneo.partidos) {
        if(partido.puntos_i == partido.puntos_j) {
            ganados.ref(partido.id_i) +=  0.5;
            perdidos.ref(partido.id_i) += 0.5;
            ganados.ref(partido.id_j) +=  0.5;
            perdidos.ref(partido.id_j) += 0.5;
        } else {
            int ganador, perdedor;
            if(partido.puntos_i > partido.puntos_j) {
                ganador = partido.id_i;
                perdedor = partido.id_j;
            } else {
                ganador = partido.id_j;
                perdedor = partido.id_i;
            }
            ganados.ref(ganador) += 1;
            perdidos.ref(perdedor) += 1;
        }
    }
    return {ganados, perdidos};
}

pair<Mat, Mat> sistema_CMM(const Torneo &torneo) {
    auto jugados = Mat::cero(torneo.equipos(), torneo.equipos());
    for(const auto &partido : torneo.partidos) {
        jugados.ref(partido.id_i, partido.id_j) += 1;
        jugados.ref(partido.id_j, partido.id_i) += 1;
    }

    auto [ganados, perdidos] = ganados_y_perdidos(torneo);

    auto sistema = Mat::cero(torneo.equipos(), torneo.equipos());
    auto b = Mat::cero(torneo.equipos());
    for(int i = 0; i < torneo.equipos(); ++i) {
        for(int j = 0; j < torneo.equipos(); ++j) {
            if(i != j) {
                if(abs(jugados(i, j)) > eps){
                    sistema.ref(i, j) = -jugados(i, j);
                }
            }
            else sistema.ref(i, j) = 2.0 + ganados(i) + perdidos(i);
        }
        b.ref(i) = 1.0 + (ganados(i) - perdidos(i)) / 2.0;
    }
    
    return {sistema, b};
}

Mat backwards_substitution(Mat &sistema) {
    auto result = Mat::cero(sistema.n);
    for (int i = sistema.n - 1; i >= 0; i--){
        ld acum = 0;
        for (int j = i + 1; j < sistema.n; j++){
            acum += ( result(j) *  sistema(i, j) );
        }
        result.ref(i) = ( sistema(i, sistema.n) - acum ) / sistema(i, i);
    }
    return result;
}


Mat forward_substitution(Mat &sistema) {
    auto result = Mat::cero(sistema.n);
    for (int i = 0; i < sistema.n; i++){
        ld acum = 0;
        for (int j = 0; j < i; j++){
            acum += ( result(j) *  sistema(i, j) );
        }
        result.ref(i) = ( sistema(i, sistema.n) - acum ) / sistema(i, i);
    }
    return result;
}


auto eliminacion_gaussiana(Mat A, Mat b) -> Mat {
    auto sistema = A.extender(b);
    // auto &sistema = A;
    for (int i = 0; i < sistema.n; ++i){
        // cerr << A.mat[i].size() << endl;
        assert(sistema(i,i) != 0);
        for (int j = i + 1; j < sistema.n; j++){
            ld coef = sistema(j, i) / sistema(i, i);
            if(abs(coef) > eps)
                sistema.operacion_2(i, j, -coef);
        }
    }
    return backwards_substitution(sistema);
}

auto cholesky(Mat A, Mat b) -> Mat {
    auto L = Mat::cero(A.n, A.n);
    for(int i = 0; i < A.n; i++){
        for(int j = 0; j <= i; j++){
            ld acum = 0;
            if ( i == j ){ // En la diagonal
                for(int k = 0; k < i;k++){
                    acum += pow(L(i, k), 2);
                }
                L.ref(i, i) = sqrt(A(i, i) - acum);
            } else {
                for(int k = 0; k < j; k++){
                    acum += L(i, k) * L(j, k);
                }
                L.ref(i, j) = (A(i, j) - acum) / L(j, j);
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
void print_rankings(const Torneo &torneo, const Mat & rankings, Of &file) {
    vector<pair<string, ld>> results;
    for (int i = 0; i < torneo.equipos(); ++i) {
        results.emplace_back(
            config.internal_id ? to_string(i) : torneo.nombres[i],
            rankings(i));
    }
    sort(results.begin(), results.end(), [&](auto a, auto b) {
        if(a.first.size() != b.first.size())
            return a.first.size() < b.first.size();
        return a < b;
    });
    for(auto [player, rank] : results) {
        if(config.show_id) {
            file << player << " ";
        }
        if(config.float_output_exact) {
            int ex = 0;
            ld res = frexp(double(rank), &ex) * pow(double(2.0), double(54));
            ex -= 54;
            file << (long long)(res + 0.5) << " " << -ex << endl;
        } else {
            file << rank << endl;
        }
    }
}

template<typename Of>
void display_matrix(pair<Mat, Mat> sistema, Of &of) {
    auto [A, b] = sistema;
    int n = A.n;
    of << n << endl;
    vector<pair<int, ld>> results;
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++ j){
            ld res = A(i, j);
            // cout << int(res + 0.5 - (res<0)) << " ";
            cout << res << " ";
        }
        cout << endl;
    }
    for(int i = 0; i < n; ++i) {
            ld res = b(i)*2;
            // cout << int(res + 0.5 - (res<0)) << " ";
            cout << res<< " ";
    }
    cout << endl;
}

Mat WP(const Torneo &torneo){

    auto result = Mat::cero(torneo.equipos()); 
    auto [ganados, perdidos] = ganados_y_perdidos(torneo);
    for (int i = 0; i < torneo.equipos(); ++i)
    {
        result.ref(i) = ganados(i)/(ganados(i)+perdidos(i));
    }
    return result;
}

ld elo_expectedScore(const ld ratingDiff){
    ld exponente = ratingDiff/(400.0);
    return 1 / (1 + pow(10,exponente));
}

Mat elo_Ratings(const Torneo &torneo, const ld K = 32){
    Mat ratings = Mat::cero(torneo.equipos()); 
    for (int i = 0; i < torneo.equipos(); i++){
        ratings.ref(i) = 1000.0;
    }
    for (auto partido_actual: torneo.partidos){
        //numeros que hacen falta
        ld scoreI,scoreJ = 0.5;
        if(partido_actual.puntos_i > partido_actual.puntos_j){scoreI = 0; scoreJ = 1;}
        else if (partido_actual.puntos_i < partido_actual.puntos_j){scoreI = 1; scoreJ = 0;}
        ld ratingDiff_J = ratings(partido_actual.id_i) - ratings(partido_actual.id_j);
        ld ratingDiff_I = - ratingDiff_J;
        ld expected_J = elo_expectedScore(ratingDiff_J);
        ld expected_I = elo_expectedScore(ratingDiff_I);

        //se actualizan los ratings 
        ratings.ref(partido_actual.id_i) += (scoreI - expected_I) * K;
        ratings.ref(partido_actual.id_j) += (scoreJ - expected_J) * K; 
    }
    return ratings;
}

void correr() {
    Torneo torneo;
    if(config.input == "-") {
        torneo = read_data(cin);
    } else {
        ifstream file(config.input);
        torneo = read_data(file);
    }
    Mat rankings;
    pair<Mat, Mat> sistema;
    auto start = chrono::steady_clock::now();
    if (config.algo == "0") {
        sistema = sistema_CMM(torneo);
        rankings = eliminacion_gaussiana(sistema.first, sistema.second);	
    }
    else if(config.algo == "1"){
        rankings = WP(torneo);
    }
    else if(config.algo == "2"){
        rankings = elo_Ratings(torneo);
    }
    else if(config.algo == "3"){
        sistema = sistema_CMM(torneo);
        rankings = cholesky(sistema.first, sistema.second);
    }
    else display_usage();
    auto end = chrono::steady_clock::now();
    long long total_time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    if(config.output != "-") {
        ofstream output(config.output);
        if(config.time) output << total_time << endl;
        else if(!config.display_matrix)
            print_rankings(torneo, rankings, output);
        else display_matrix(sistema, output);
    } else {
        if(config.time) cout << total_time << endl;
        else if(!config.display_matrix)
            print_rankings(torneo, rankings, cout);
        else display_matrix(sistema, cout);
    }
}
