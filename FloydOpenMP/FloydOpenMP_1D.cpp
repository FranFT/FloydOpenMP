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

	// Control de errores de la entrada.
	if (argc != 3){
		cerr << "Sintaxis: " << argv[0] << " <archivo de grafo> <num_threads>" << endl;
		return(-1);
	}

	cout << "Fichero: '" << argv[1] << "' con numero de hebras = " << atoi(argv[2]) << endl;

	// Si la entrada es correcta creamos el objeto Grafo y leemos el fichero.
	Graph G;
	G.lee(argv[1]);

	// Fijamos el número de elementos.
	int nverts = G.vertices;

	// Si se ha podido leer el fichero, obtenemos el número de hebras que ejecutarán el problema.
	int num_threads = atoi(argv[2]);

	// Si el número de hebras es mayor que el máximo se escoge el máximo.
	if (num_threads > omp_get_max_threads()) {
		cout << "Aviso: Numero de hebras cambiado de " << num_threads << " a " << omp_get_max_threads() << " porque excede el maximo." << endl;
		num_threads = omp_get_max_threads();
	}

	// Si el número de vertices no es múltiplo del número de hebras se aborta.
	if (nverts%num_threads != 0){
		cerr << "El numero de vertices no es divisible entre numero de hebras" << endl;
		return(-1);
	}

	// Fijamos el número de hebras que ejecutarán el algoritmo de Floyd.
	omp_set_num_threads(num_threads);


	//	double t = MPI::Wtime();
	// BUCLE PPAL DEL ALGORITMO
	int i, j, k, vikj;
	for (k = 0; k<nverts; k++)
	{
		for (i = 0; i<nverts; i++)
			for (j = 0; j<nverts; j++)
				if (i != j && i != k && j != k)
				{
					vikj = G.arista(i, k) + G.arista(k, j);
					vikj = min(vikj, G.arista(i, j));
					G.inserta_arista(i, j, vikj);
				}
	}
	//	t = MPI::Wtime() - t;


	cout << endl << "El grafo con las distancias de los caminos mas cortos es:" << endl << endl;
	G.imprime();
	//	cout << "Tiempo gastado= " << t << endl << endl;
}