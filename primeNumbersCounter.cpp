#include <pthread.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include "utils.h"

#define N 	6		//tamanho da matriz
#define SEED 	1418 		//altere essa semente para uma de sua escolha
#define XCEIL  	3		//Tamanho do macrobloco no eixo x
#define YCEIL  	3		//Tamanho do macrobloco no eixo y

using namespace std;

unsigned count = 0;

typedef struct _macrobloco{
	int index_x;
	int index_y;
	unsigned **matrix;
}macrobloco;

void printMatrix(unsigned **prime){
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++)
			cout << prime[i][j] << "\t";
		cout << "\n";
	}
}
void *countPrimes(void *blck){
	macrobloco *block = (macrobloco *)blck;
	unsigned **matrix = &(block -> matrix[block -> index_y]);
	
	for(int i = 0; i < YCEIL; i++){
		for(int j = block -> index_x; j < block -> index_x + XCEIL; j++)
			if(primeNumber(matrix[i][j]))
				count++;
	}
	//pthread_exit(NULL);
}

int main(){
	srand(SEED);
	unsigned **prime_numbers;
	unsigned n_prime_numbers = 0;
	int NUM_THREADS = (N/YCEIL)*(N/XCEIL);	
	pthread_t threads[NUM_THREADS];
	int rc, t = 0;
	
	prime_numbers = allocMatrix(N);
	fillMatrix(prime_numbers, N);
	
	macrobloco *m = new macrobloco;
	m -> matrix = prime_numbers;
	printMatrix(prime_numbers);
	int t1 = 0, t2 = 0;
	for(int i = 0; i < N; i+=YCEIL)
		for(int j = 0; j < N; j+=XCEIL){
			m -> index_x = j;
			m -> index_y = i;
			countPrimes((void *)m);
			rc = pthread_create(&threads[t], NULL, countPrimes, (void *)m);
			//pthread_join(threads[i], NULL);
			if(rc){
				cout << "Erro " << rc << "\n";
				exit(-1);			
			}
			cout << ++t << "a thread criada\n"; 
		}


	cout << count << " números primos encontrados \n" << t2 - t1 << " segundos de processamento\n";

	pthread_exit(NULL);
	deallocMatrix(&prime_numbers, N);
//	cout << n_prime_numbers << "\t" << end - beg << "\n" << te - ti;
}
