#include "stdafx.h"

#include <omp.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include "Graph.h"

int main(int argc, char* argv[])
{
	std::cout << "Floyd 1D." << std::endl;

	int num_threads = 0;
	int thread_id = 0;
	int* I;
	Graph G;

	// Control de errores de la entrada.
	if (argc != 3){
		std::cerr << "Sintaxis: " << argv[0] << " <archivo de grafo> <num_threads>" << std::endl;
		return(-1);
	}
	else {
		// Compruebo si el número de hebras deseado es correcto (num_hebras >= 1)
		num_threads = atoi(argv[2]);

		if (num_threads < 1) {
			std::cerr << "Numero de hebras no valido. Debe ser mayor o igual que 1." << std::endl;
			return(-1);
		}
	}

	std::cout << "Fichero: '" << argv[1] << "' con numero de hebras = " << num_threads << std::endl;

	// Si la entrada es correcta leemos el fichero.
	G.lee(argv[1]);

	// Fijamos el número de filas de I que tendrá cada proceso.
	const int num_vertices = G.get_vertices();
	const int num_filas = num_vertices / num_threads;
	const int nelementos = num_filas * num_vertices;

	// Si el número de vertices no es múltiplo del número de hebras se aborta.
	if (num_vertices%num_threads != 0){
		std::cerr << "El numero de vertices no es divisible entre numero de hebras" << std::endl;
		return(-1);
	}

	// Fijamos el número de hebras que ejecutarán el algoritmo de Floyd.
	omp_set_num_threads(num_threads);

	// Iniciamos la región paralela. Cada thread tendrá una copia propia de las siguientes variables:
	//	> thread_id
	//	> I
	//	> i, j, k
	//	> i_global
	//	> vikj
	int i, j, k, vikj;
	int * i_global;
	int * fila_i;

	// Guardamos el valor del relog en segundos.
	double t = omp_get_wtime();

#pragma omp parallel private(thread_id, I, i, j, k, i_global, vikj, fila_i)
	{
		// Fijamos el identificador de la hebra.
		thread_id = omp_get_thread_num();

		// Reservamos memoria para la submatriz I.
		I = new int[nelementos];

		// Reservamos memoria para los coeficientes de 'i_global',
		// De esta manera no hay que volver a calcularlos de nuevo cada iteración k-ésima.
		i_global = new int[num_filas];

		// Reservamos memoria para el vector fila_i, que nos permitirá ahorrar cálculos.
		// Almacena la posición de memoria que corresponde a la fila i.
		fila_i = new int[num_filas];

		// Calculamos y almacenamos dichos coeficientes.
		for (i = 0; i < num_filas; i++){
			i_global[i] = thread_id*num_filas + i;
			fila_i[i] = i*num_vertices;
		}

		// Copiamos en I, la submatriz correspondiente de A.
		for (i = 0; i < num_filas; i++)
			for (j = 0; j < num_vertices; j++)
				I[i*num_vertices + j] = G.get_elemento_matriz_A(thread_id*num_filas + i, j);

		// Bucle principal del algoritmo.
		for (k = 0; k < num_vertices; k++)
			for (i = 0; i < num_filas; i++)
				for (j = 0; j < num_vertices; j++)
					if (i_global[i] != j && i_global[i] != k && j != k)
					{
						vikj = I[fila_i[i] + k] + G.get_elemento_matriz_A(k, j);
						vikj = std::min(vikj, I[fila_i[i] + j]);
						I[fila_i[i] + j] = vikj;
						G.inserta_arista(i_global[i], j, vikj);
						#pragma omp barrier 
					}

		// Liberamos la memoria reservada.
		delete[] I;
		delete[] i_global;
		delete[] fila_i;
	}

	// Calculamos el tiempo en segundos que tardó en finalizar el algoritmo.
	t = omp_get_wtime() - t;


	std::cout << std::endl << "El grafo con las distancias de los caminos mas cortos es:" << std::endl << std::endl;
	G.imprime();
	std::cout << "Tiempo gastado= " << t << std::endl << std::endl;
}