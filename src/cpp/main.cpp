#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
using mat = vector<vector<double>>;

mat readData0(string path){
	ifstream infile(path);

	int equipos,partidos;
	infile >> equipos >> partidos;
	mat result(equipos, vector<double>(equipos + 1,0));
	for (int i = 0; i < equipos; ++i)
	{
		//inicalizo la diagonal y el vector B antes de acumular
		result[i][i] = 2;
		result[i][equipos] = 1;
	}

	for (int i = 0; i < partidos; ++i)
	{
		//generamos el sistema acumulando los valores a medida que leemos
		int identificador,equipo1,equipo2,puntaje1,puntaje2;
		infile >> identificador >> equipo1 >> puntaje1 >> equipo2 >> puntaje2;
		if (puntaje1 >= puntaje2){
			result[equipo1 - 1][equipos] += 0.5;
			result[equipo2 - 1][equipos] -= 0.5;
		}else{
			result[equipo1 - 1][equipos] -= 0.5;
			result[equipo2 - 1][equipos] += 0.5;
		}
		result[equipo1 - 1][equipo2 - 1] -= 1;
		result[equipo2 - 1][equipo1 - 1] -= 1;
		result[equipo1 - 1][equipo1 - 1] += 1;
		result[equipo2 - 1][equipo2 - 1] += 1;
	}

	return(result);
	//todo
}


vector<double> eliminacionGaussiana(mat & sistema){
	for (int i = 0; i < sistema.size() - 1; ++i)
	{
		for (int j = i + 1; j < sistema.size(); j++)
		{
			double coef = sistema[j][i] / sistema[i][i];

			for (int k = i; k < sistema[0].size(); k++)
			{
				sistema[j][k] = sistema[j][k] - (sistema[i][k] * coef);
			}
		}
	}
	vector<double> result(sistema.size(),0);
	for (int i = result.size() -1; i >= 0; i--)
	{
		double acum = 0;
		for (int j = i + 1; j < sistema.size(); j++)
		{
			acum += ( result[j] *  sistema[i][j]);
		}
		result[i] = ( sistema[i][sistema.size()] - acum ) / sistema[i][i];
	}

	return result;

}

void printRankings(vector<double> & rankings, string path){
	ofstream file(path);
	for (int i = 0; i < rankings.size(); ++i)
	{
		file << rankings[i] << endl;
	}
}

vector<double> readData1(string path){
	ifstream infile(path);
	int equipos,partidos;
	infile >> equipos >> partidos;

	vector<double> res(equipos,0);
	vector<double> partidos_por_equipo(equipos, 0);

	for (int i = 0; i < partidos; ++i)
	{
		int identificador,equipo1,equipo2,puntaje1,puntaje2;
		infile >> identificador >> equipo1 >> puntaje1 >> equipo2 >> puntaje2;

		if(puntaje1 > puntaje2){
			res[equipo1 - 1] ++;
		}else{
			res[equipo2 - 1] ++;
		}
		partidos_por_equipo[equipo1 - 1] ++;
		partidos_por_equipo[equipo2 - 1] ++;
	}
	for (int i = 0; i < equipos; ++i)
	{
		res[i] = res[i]/partidos_por_equipo[i];
	}
	return res;
}

 
int main(int argc, char **argv){
	vector<double> rankings;
    if (*argv[1] == '0'){
		mat sistema = readData0(argv[2]);
		rankings = eliminacionGaussiana(sistema);	
	}
	else if(*argv[1] == '1'){
		rankings = readData1(argv[2]);
	}
	else if(*argv[1] == '2'){
		//todo
	}else{
		cout << "Uso: metodo, inputpath, outputpath" <<  endl;
	}
	printRankings(rankings,argv[3]);
	return 0;
}