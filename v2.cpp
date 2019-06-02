#include <cstdlib>
#include <ctime>
#include <iostream>
#include <pthread.h>
#include "utils.h"

using namespace std;

#define N				10000
#define SEED			1408
#define CEIL_X			2
#define CEIL_Y			2
#define	NUM_THREADS 	2

typedef struct macrobloco_{
	long				thread_id;
	unsigned 			index_x;
	unsigned 			index_y;
}macrobloco;

pthread_mutex_t 		mutex_counter;
pthread_mutex_t 		mutex_indexes;
unsigned 				**matrix;
unsigned				last_x;
unsigned				last_y;
unsigned long			count = 0;

void printMatrix(){
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++)
			cout << matrix[i][j] << "\t";
		cout << "\n";
	}
}

unsigned long countPrimes(unsigned **Matrix, unsigned beg_x, unsigned end_x, unsigned end_y)
{
	unsigned long sum = 0;

	for(int i = 0; i < end_y; i++)
 		for(int j = beg_x; j < end_x; j++)
			sum += primeNumber(Matrix[i][j]);

	return sum;
}

void *work(void *m1)
{
	macrobloco *m 	= (macrobloco *) m1;
	unsigned long s = 0;

	while(true)
	{		
		s += countPrimes(&matrix[m -> index_y], m -> index_x, CEIL_X, CEIL_Y);
		
		pthread_mutex_lock(&mutex_indexes);

		if(last_x + CEIL_X < N)
		{
			m -> index_x 	= last_x + CEIL_X;
			last_x	   		= last_x + CEIL_X;

			pthread_mutex_unlock(&mutex_indexes);			
		}
		else if(last_y + CEIL_Y < N)
		{
			m -> index_y = last_y + CEIL_Y;
			last_y 		 = last_y + CEIL_Y;
			m -> index_x = 0; 
			last_x 		 = 0;
			pthread_mutex_unlock(&mutex_indexes);
		}
		else
		{
			pthread_mutex_unlock(&mutex_indexes);
			break;
		}
	}
	pthread_mutex_lock(&mutex_counter);
	count += s;	
	pthread_mutex_unlock(&mutex_counter);

	pthread_exit(NULL);
}

int main()
{
	int 				rc;
	int					t1, t2, t3, t_id;
	int					x;
	macrobloco 			*m;	
	pthread_t 			*threads;	

	m 		= new macrobloco [NUM_THREADS];
	threads = new pthread_t  [NUM_THREADS];

	matrix	= allocMatrix(N);
			  fillMatrix(matrix, N);
//			  printMatrix();

	pthread_mutex_init(&mutex_counter, NULL);		
	pthread_mutex_init(&mutex_indexes, NULL);
	
	t1 = time(NULL);
	x  = countPrimes(matrix, 0, N, N);	
	t2 = time(NULL) - t1;
	cout << "================\n";
	t1 = time(NULL);
	for(int i = 0, t_id = 0; i < NUM_THREADS; i++, t_id++)
	{
		m[t_id].index_y 	= 0;
		m[t_id].index_x 	= i * CEIL_X;
		m[t_id].thread_id 	= t_id;
		last_y 		 		= m[t_id].index_y;
		last_x  	 		= m[t_id].index_x;
		
		rc = pthread_create(&threads[t_id], NULL, work, &m[t_id]);
		
		if(rc)
		{
			cout << "Erro: " << rc << "\n";
			exit(-1);
		}
	}

	for(int i = 0; i < NUM_THREADS; i++)
		(void) pthread_join(threads[i], NULL);

	t3 = time(NULL) - t1;

	cout << "SERIAL:\n\t" << x << " números primos encontrados.\n\t"
		 << t2 << " segundos de processamento\n\n";

	cout << "PARALELA:\n\t" << x << " números primos encontrados.\n\t"
		 << t3 << " segundos de processamento\n\n";


	delete m;
	delete threads;
	deallocMatrix(&matrix, N);
	pthread_exit(NULL);
}			
