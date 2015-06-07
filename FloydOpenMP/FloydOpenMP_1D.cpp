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
	G.imprime();

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

	// Guardamos el valor del relog en segundos.
	double t = omp_get_wtime();

	// Iniciamos la región paralela. Cada thread tendrá una copia propia de las siguientes variables:
	//	> thread_id
	//	> I
	//	> i, j, k
	//	> i_global
	//	> vikj
	
	omp_lock_t salida;
	omp_init_lock(&salida);

	int i, j, k, i_global, vikj;
	#pragma omp parallel private(thread_id, I, i, j, k, i_global, vikj)
	{
		// Fijamos el identificador de la hebra.
		thread_id = omp_get_thread_num();		

		// Reservamos memoria para la submatriz I.
		I = new int[nelementos];

		// Copiamos en I, la submatriz correspondiente de A.
		for (i = 0; i < num_filas; i++)
			for (j = 0; j < num_vertices; j++)
				I[i*num_vertices + j] = G.get_elemento_matriz_A(thread_id*num_filas + i, j);//A[(thread_id*num_filas + i)*num_vertices + j];

		for (k = 0; k < num_vertices; k++)
		{
			for (i = 0; i < num_filas; i++)
			{
				i_global = thread_id*num_filas + i;

				for (j = 0; j < num_vertices; j++)
				{
					if (i_global != j && i_global != k && j != k)
					{
						vikj = I[i*num_vertices + k] + G.get_elemento_matriz_A(k, j);
						vikj = std::min(vikj, I[i*num_vertices + j]);
						I[i*num_vertices + j] = vikj;
						G.inserta_arista(i_global, j, vikj);
						#pragma omp barrier 
					}
				}
			}
		}

		omp_set_lock(&salida);
		std::cout << "Thread " << thread_id << ":" << std::endl;
		for (i = 0; i < num_filas; i++){
			for (j = 0; j < num_vertices; j++)
				std::cout << I[i*num_vertices + j] << ", ";
			std::cout << std::endl;
		}
		std::cout << std::endl;
		omp_unset_lock(&salida);

		// Liberamos la memoria reservada.
		delete[] I;
	}

	omp_destroy_lock(&salida);

	// Calculamos el tiempo en segundos que tardó en finalizar el algoritmo.
	t = omp_get_wtime() - t;


	std::cout << std::endl << "El grafo con las distancias de los caminos mas cortos es:" << std::endl << std::endl;
	G.imprime();
	std::cout << "Tiempo gastado= " << t << std::endl << std::endl;
}

//for (k = 0; k < num_vertices; k++) {
//	for (i = 0; i < num_filas; i++) {
//		i_global = thread_id*num_filas + i;
//		omp_set_lock(&salida);
//		std::cout << "Thread " << thread_id << " i_global: " << i_global << std::endl;
//		omp_unset_lock(&salida);
//		for (j = 0; j < num_vertices; j++){
//			if (i_global!=j && i_global!=k && j!=k) {
//				//vikj = I[i*num_vertices + j] + I[k*num_vertices + j];//G.get_elemento_matriz_A(k,j);
//				//vikj = std::min(vikj, I[i*num_vertices + j]);
//				//G.inserta_arista(i, j, vikj);
//				//I[i*num_vertices + j] = vikj;
//				//std::cout << k << " " << i << " " << j << " " << vikj << std::endl;
//			}
//		}
//	}
//	//omp_set_lock(&salida);
//	//std::cout << "Thread " << thread_id << ":" << std::endl;
//	//for (i = 0; i < num_filas; i++){
//	//	for (j = 0; j < num_vertices; j++)
//	//		std::cout << I[i*num_vertices + j] << ", ";
//	//	std::cout << std::endl;
//	//}
//	//std::cout << std::endl;
//	//omp_unset_lock(&salida);
//}