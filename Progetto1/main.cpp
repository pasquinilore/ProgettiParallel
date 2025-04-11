#include <iostream>
#include <cmath>
#include <random>
#include <omp.h>
using namespace std;

static const int NCENTROIDI = 20;
static const int NPUNTI = 100;
static const int SEED = 624; //se mersenne ci mette troppo passa a default engine o altro algoritmo ad alto periodo


struct centroide{
	double x, y;
	int numpunti = 0;
};

struct punto{
	double x, y;
	int appartenenza = -1; //centroide a cui il punto appartiene
};

//funzioni di generazione
centroide* generacent(mt19937 gen, uniform_real_distribution<double> urd){
	auto *centroidi = new centroide[NCENTROIDI];
	#pragma omp parallel for
	for (int i = 0; i < NCENTROIDI; i++){
		centroidi[i].y = urd(gen);
		centroidi[i].x = urd(gen);
	}
	return centroidi;
}

punto* generapunti(mt19937 gen, uniform_real_distribution<double> urd){
	auto *punti = new punto[NPUNTI];
	#pragma omp parallel for
	for (int i = 0; i < NPUNTI; i++){
		punti[i].x = urd(gen);
		punti[i].y = urd(gen);
	}
	return punti;
}


int main() {
}