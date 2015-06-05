#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <omp.h>
#include "Graph.h"
using namespace std;

int main(int argc, char* argv[])
{
	cout << "Floyd 1D." << endl;

	int num_threads = 0;
	int num_vertices = 0;
	int thread_id = 0;
	Graph G;

	// Control de errores de la entrada.
	if (argc != 3){
		cerr << "Sintaxis: " << argv[0] << " <archivo de grafo> <num_threads>" << endl;
		return(-1);
	}
	else {
		// Compruebo si el número de hebras deseado es correcto (num_hebras >= 1)
		num_threads = atoi(argv[2]);

		if (num_threads < 1) {
			cerr << "Numero de hebras no valido. Debe ser mayor o igual que 1." << endl;
			return(-1);
		}
	}

	cout << "Fichero: '" << argv[1] << "' con numero de hebras = " << num_threads << endl;

	// Si la entrada es correcta leemos el fichero.
	G.lee(argv[1]);

	num_vertices = G.vertices;

	// Si el número de vertices no es múltiplo del número de hebras se aborta.
	if (num_vertices%num_threads != 0){
		cerr << "El numero de vertices no es divisible entre numero de hebras" << endl;
		return(-1);
	}

	// Fijamos el número de hebras que ejecutarán el algoritmo de Floyd.
	omp_set_num_threads(num_threads);

	// Guardamos el valor del relog en segundos.
	double t = omp_get_wtime();

	// BUCLE PPAL DEL ALGORITMO
	int i, j, k, vikj;
	for (k = 0; k<num_vertices; k++)
	{
		for (i = 0; i<num_vertices; i++)
			for (j = 0; j<num_vertices; j++)
				if (i != j && i != k && j != k)
				{
					vikj = G.arista(i, k) + G.arista(k, j);
					vikj = min(vikj, G.arista(i, j));
					G.inserta_arista(i, j, vikj);
				}
	}

	// Calculamos el tiempo en segundos que tardó en finalizar el algoritmo.
	t = omp_get_wtime() - t;


	cout << endl << "El grafo con las distancias de los caminos mas cortos es:" << endl << endl;
	G.imprime();
	cout << "Tiempo gastado= " << t << endl << endl;
}