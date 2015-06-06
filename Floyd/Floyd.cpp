#include "stdafx.h"

#include <omp.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include "Graph.h"

int main(int argc, char* argv[])
{
	std::cout << "Floyd Secuencial." << std::endl;

	Graph G;

	// Control de errores de la entrada.
	if (argc != 2){
		std::cerr << "Sintaxis: " << argv[0] << " <archivo de grafo> <num_threads>" << std::endl;
		return(-1);
	}

	// Si la entrada es correcta leemos el fichero.
	G.lee(argv[1]);
	G.imprime();

	// Fijamos el número de vertices.
	const int num_vertices = G.get_vertices();

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
					vikj = std::min(vikj, G.arista(i, j));
					G.inserta_arista(i, j, vikj);
				}
	}

	// Calculamos el tiempo en segundos que tardó en finalizar el algoritmo.
	t = omp_get_wtime() - t;


	std::cout << std::endl << "El grafo con las distancias de los caminos mas cortos es:" << std::endl << std::endl;
	G.imprime();
	std::cout << "Tiempo gastado= " << t << std::endl << std::endl;
}