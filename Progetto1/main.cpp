#include <iostream>
#include <cmath>
#include <random>
#include <omp.h>
using namespace std;

static const int NCENTROIDI = 20;
static const int NPUNTI = 10;
static const int MAXTHREADS = 1;


struct centroide{
	double x, y, z;
	int numpunti = 0;
};

struct punto{
	double x, y, z;
	int appartenenza = -1; //centroide a cui il punto appartiene
};

//funzioni di generazione
centroide* generacent(mt19937 gen, uniform_real_distribution<double> urd){
	auto *centroidi = new centroide[NCENTROIDI];
	for (int i = 0; i < NCENTROIDI; i++){
		centroidi[i].y = urd(gen);
		centroidi[i].x = urd(gen);
		centroidi[i].z = urd(gen);
	}
	return centroidi;
}

punto* generapunti(mt19937 gen, uniform_real_distribution<double> urd){
	auto *punti = new punto[NPUNTI];
	for (int i = 0; i < NPUNTI; i++){
		punti[i].x = urd(gen);
		punti[i].y = urd(gen);
		punti[i].z = urd(gen);
	}
	return punti;
}
//funzioni utili
double distanza(double x1, double x2, double y1, double y2, double z1,  double z2){
	return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2) + pow(z1+z2, 2));
}

int main() {
	bool permutato = true;
	double maxdist = INFINITY;
	double dist;
	int temp;
	mt19937 mt{random_device{}()};
	uniform_real_distribution<double> urd(0.0, 100.0);
	punto* p = generapunti(mt, urd);
	centroide* c = generacent(mt, urd);
	
#pragma omp parallel default(none) shared(p, c, NCENTROIDI, NPUNTI, permutato, maxdist, dist, temp) num_threads(MAXTHREADS)
{
		while(permutato){
			#pragma omp barrier
			#pragma omp single
			{
			permutato = false;
			}
		#pragma omp for
			for (int i = 0; i < NPUNTI; i++){
				maxdist = INFINITY;
				temp = -1;
				
				for (int j = 0; j < NCENTROIDI; j++){
					dist = distanza(p[i].x, c[j].x, p[i].y, c[j].y, p[i].z, c[j].z);
					printf("Distanza tra punto %d e centroide %d = %f \n\n", i,j,dist);
					if (dist < maxdist){
						maxdist = dist;
						temp = j;
					}
				}
				
				if (temp != p[i].appartenenza){
					printf("Thread %d ha permutato punto %d in centroide %d\n", omp_get_thread_num(), i, temp);
					permutato = true;
					p[i].appartenenza = temp;
					c[temp].numpunti++;
					}
				
			}
		}
	}
	
	
for (int i = 0; i < NPUNTI; i++){
	printf("punto %d ha x = %f, y = %f, z = %f e appartiene al centroide %d\n", i, p[i].x, p[i].y, p[i].z, p[i].appartenenza);
}
for (int j = 0; j < NCENTROIDI; j++){
		printf("centroide %d ha x = %f, y = %f, z = %f e ha %d punti\n", j, c[j].x, c[j].y, c[j].z, c[j].numpunti);
}
delete[](p);
delete[](c);
}