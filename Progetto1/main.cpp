#include <iostream>
#include <cmath>
#include <random>
#include <omp.h>
#include <chrono>
using namespace std;

static const int NCENTROIDI = 30;
static const int NPUNTI = 2000;
static const int MAXTHREADS = 1;

struct centroide {
	double x = 0; 
	double y = 0; 
	double z = 0;
	int numpunti = 0;
};

struct punto {
	double x = 0;
	double y = 0;
	double z = 0;
	int appartenenza = -1; //centroide a cui il punto appartiene
};

//funzioni di generazione

centroide* generacent(mt19937 &gen, uniform_real_distribution<double> &urd) {
	auto *centroidi = new centroide[NCENTROIDI];
	for (int i = 0; i < NCENTROIDI; i++) {
		centroidi[i].y = urd(gen);
		centroidi[i].x = urd(gen);
		centroidi[i].z = urd(gen);
		centroidi[i].numpunti = 0;
	}
	return centroidi;
}

punto* generapunti(mt19937 &gen, uniform_real_distribution<double> &urd) {
	auto *punti = new punto[NPUNTI];
	for (int i = 0; i < NPUNTI; i++) {
		punti[i].x = urd(gen);
		punti[i].y = urd(gen);
		punti[i].z = urd(gen);
		punti[i].appartenenza = -1;
	}
	return punti;
}

//funzioni di utilità

double distanza(double x1, double x2, double y1, double y2, double z1,  double z2) {
	return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2) + pow(z1-z2, 2));
}

//main

int main() {
	bool permutato = true;
	mt19937 mt{9};
	uniform_real_distribution<double> urd(0.0, 100.0);
	punto *p = generapunti(mt, urd);
	centroide *c = generacent(mt, urd);
	for(int i = 0; i < NCENTROIDI; i++){
		printf("Centroide %d ha x:%f y:%f z:%f\n", i, c[i].x, c[i].y, c[i].z);
	}
	printf("\n\n");	
	chrono::steady_clock::time_point inizio = chrono::steady_clock::now();
	#pragma omp parallel default(none) shared(p, c, NCENTROIDI, NPUNTI, permutato) num_threads(MAXTHREADS)
	{
		while(permutato) {
			centroide tcent[NCENTROIDI];
			
			#pragma omp barrier
			
			#pragma omp single
			permutato = false;
			
			#pragma omp for
			for (int i = 0; i < NPUNTI; i++) {
				double mindist = INFINITY;
				int temp = -1;
				
				
				for (int j = 0; j < NCENTROIDI; j++) {
					double dist = distanza(p[i].x, c[j].x, p[i].y, c[j].y, p[i].z, c[j].z);
					if (dist < mindist) {
						mindist = dist;
						temp = j;
					}
				}

				if (temp != p[i].appartenenza) {
					p[i].appartenenza = temp;
					permutato = true;
				}

				tcent[temp].x += p[i].x;
				tcent[temp].y += p[i].y;
				tcent[temp].z += p[i].z;
				tcent[temp].numpunti++;
			}


			if (permutato) {
				#pragma omp single
				c = new centroide[NCENTROIDI];

				for (int l = 0; l < NCENTROIDI; l++) {
					#pragma omp atomic update
					c[l].x += tcent[l].x;
					#pragma omp atomic update
					c[l].y += tcent[l].y;
					#pragma omp atomic update
					c[l].z += tcent[l].z;
					#pragma omp atomic update
					c[l].numpunti += tcent[l].numpunti;
				}


				#pragma omp barrier

				#pragma omp for nowait
				for(int n = 0; n < NCENTROIDI; n++) {
					c[n].x /= c[n].numpunti;
					c[n].y /= c[n].numpunti;
					c[n].z /= c[n].numpunti;
				}

			}
		}
	}
	chrono::steady_clock::time_point fine = chrono::steady_clock::now();
	for(int i = 0; i < NCENTROIDI; i++){
		printf("Centroide %d ha x:%f y:%f z:%f e contiene %i punti\n", i, c[i].x, c[i].y, c[i].z, c[i].numpunti);
	}
	cout << "Tempo d'esecuzione: " << (double)(chrono::duration_cast<std::chrono::milliseconds>(fine - inizio).count())/1000 << " secondi" << endl;
	delete[](c);
	delete[](p);
}