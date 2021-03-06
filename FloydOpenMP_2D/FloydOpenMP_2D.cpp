#include "stdafx.h"

#include <omp.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include "Graph.h"

int main(int argc, char* argv[])
{
	std::cout << "Floyd 2D." << std::endl;

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
		// Compruebo si el n�mero de hebras deseado es correcto.
		num_threads = atoi(argv[2]);

		if (num_threads < 1 || sqrt(num_threads) != ceil(sqrt(num_threads))) {
			std::cerr << "Numero de hebras no valido. Debe ser positivo y tener raiz cuadrada entera." << std::endl;
			return(-1);
		}
	}

	std::cout << "Fichero: '" << argv[1] << "' con numero de hebras = " << num_threads << std::endl;

	// Si la entrada es correcta leemos el fichero.
	G.lee(argv[1]);

	// Fijamos el n�mero de filas de I que tendr� cada proceso.
	const int num_vertices = G.get_vertices();
	const int raiz_t = static_cast<int>(sqrt(num_threads));
	const int num_filas = num_vertices / raiz_t; 
	const int nelementos = num_filas*num_filas;

	// Si el n�mero de v�rtices no es m�ltiplo del n�mero de hebras se aborta.
	if (num_vertices % raiz_t != 0){
		std::cerr << "El numero de vertices no es divisible entre la raiz del numero de hebras" << std::endl;
		return(-1);
	}

	// Fijamos el n�mero de hebras que ejecutar�n el algoritmo de Floyd.
	omp_set_num_threads(num_threads);

	// Guardamos el valor del relog en segundos.
	double t = omp_get_wtime();

	// Iniciamos la regi�n paralela. Cada thread tendr� una copia propia de las siguientes variables:
	//	> thread_id
	//	> I
	//	> i, j, k
	//	> i_global
	//	> vikj
	int i, j, k, vikj;
	int fila_malla_threads, col_mall_threads;
	int *i_global, *j_global;
	int **posicion;

	omp_lock_t salida;
	omp_init_lock(&salida);


//#pragma omp parallel private(thread_id, I, i, j, k, i_global, j_global, vikj, fila_i, fila_malla_threads, col_mall_threads)
#pragma omp parallel private(thread_id, I, i, j, k, vikj, fila_malla_threads, col_mall_threads, i_global, j_global, posicion)
	{
		// Fijamos el identificador de la hebra.
		thread_id = omp_get_thread_num();

		// Calculamos la fila y columna en la malla de threads de nuestro mapeado.
		fila_malla_threads = thread_id / raiz_t;
		col_mall_threads = thread_id % raiz_t;

		// Reservamos memoria para la submatriz I.
		I = new int[nelementos];

		// Reservamos memoria para los coeficientes de 'i_global' y 'j_global',
		// De esta manera no hay que volver a calcularlos de nuevo cada iteraci�n k-�sima.
		i_global = new int[num_filas];
		j_global = new int[num_filas];

		// Reservamos memoria para el vector fila_i, que nos permitir� ahorrar c�lculos.
		// Almacena la posici�n de memoria que corresponde a la fila i.
		posicion = new int*[num_filas];
		for (i = 0; i < num_filas; i++)
			posicion[i] = new int[num_filas];

		// Inicializamos los vectores que contienen las 'i' y 'j' globlales.
		for (i = 0; i < num_filas; i++){
			i_global[i] = (fila_malla_threads*num_filas) + i;
			j_global[i] = (col_mall_threads*num_filas) + i;
			for (j = 0; j < num_filas; j++)
				posicion[i][j] = num_filas * i + j;
		}

		// Copiamos en I, la submatriz correspondiente de A.
		for (i = 0; i < num_filas; i++)
			for (j = 0; j < num_filas; j++)
				I[posicion[i][j]] = G.get_elemento_matriz_A(i_global[i], j_global[j]);

		// Bucle principal del algoritmo.
		for (k = 0; k < num_vertices; k++){
			for (i = 0; i < num_filas; i++){
				for (j = 0; j < num_filas; j++){
					if (i_global[i] != j_global[j] && i_global[i] != k && j_global[j] != k)
					{
						vikj = G.get_elemento_matriz_A(i_global[i], k) + G.get_elemento_matriz_A(k, j_global[j]);
						vikj = std::min(vikj, I[posicion[i][j]]);
						I[posicion[i][j]] = vikj;
						G.inserta_arista(i_global[i], j_global[j], vikj);
					}
				}
				#pragma omp barrier
			}
		}

		// Liberamos la memoria reservada.
		delete[] I;
		delete[] i_global;
		delete[] j_global;
		
		for (i = 0; i < num_filas; i++)
			delete[] posicion[i];
		delete[] posicion;
	}

	omp_destroy_lock(&salida);

	// Calculamos el tiempo en segundos que tard� en finalizar el algoritmo.
	t = omp_get_wtime() - t;


	std::cout << std::endl << "El grafo con las distancias de los caminos mas cortos es:" << std::endl << std::endl;
	G.imprime();
	std::cout << "Tiempo gastado= " << t << std::endl << std::endl;
}