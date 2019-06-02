#include <pthread.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include "utils.h"

using namespace std;

//DEFINIÇÕES
#define N 		50		//tamanho da matriz
#define SEED 	1408 		//altere essa semente para uma de sua escolha
#define XCEIL  	2			//Tamanho do macrobloco no eixo x
#define YCEIL  	2			//Tamanho do macrobloco no eixo y

typedef struct _macrobloco{
	int thread_id;
	int index_x;
	int index_y;
}macrobloco;

//VARIÁVEIS GLOBAIS
pthread_mutex_t mutex_counter;
pthread_mutex_t mutex;
unsigned **matrix;
unsigned lastX = 0;
unsigned lastY = 0;
unsigned long count = 0;

//FUNÇÕES

void printMatrix(unsigned **prime){			//IMPRIME A MATRIZ NA TELA
	for(int i = 0; i < N; i++){					//APENAS PARA MATRIZES PEQUENAS
		for(int j = 0; j < N; j++){
			cout << prime[i][j] << "\t";
		}
		cout << "\n";
	}
}

int countPrimesSerial(unsigned **Matrix){	//CONTA OS PRIMOS DA MATRIZ TODA
	int s = 0;
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++)
			s += primeNumber(Matrix[i][j]);
	}
	return s;
}

unsigned countPrimes(void *m1){				//CONTA OS PRIMOS DE APENAS UM BLOCO
	macrobloco *m = (macrobloco *)m1;
	unsigned long yay = 0;

	for(int i = 0; i < YCEIL && m -> index_y + i < N; i++)
		for(int j = 0; j < XCEIL && m -> index_x + j < N; j++)
			if(primeNumber(matrix[i + m -> index_y][j + m -> index_x]))
				yay++;	
	
	return yay;
}


void *work(void *m1){
	macrobloco *m = (macrobloco *)m1;			//ESTRUTURA COM OS PARÂMETROS DA FUNÇÃO
	unsigned l = 0, s;							//AUXILIARES PRA FIM DE DEPURAÇÃO
	while(true){
		s = countPrimes((void *)m);		//RESULTADO DO MACROBLOCO

		pthread_mutex_lock(&mutex_counter);				//ENTRANDO NA REGIÃO CRITICA PRA CONSULTAR/ALTERAR AS GLOBAIS
		count += s;								//SOMA RESULTADO DO MACROBLOCO AO CONTADOR GLOBAL
		pthread_mutex_unlock(&mutex_counter);		
		pthread_mutex_unlock(&mutex);
		if(lastX + XCEIL < N){					//SE AINDA HOUVER COLUNA PRA SER PROCESSADA,
			m -> index_x = lastX + XCEIL;			//VAMO LÁ
			lastX += XCEIL;
			pthread_mutex_unlock(&mutex);		//SAI DA REGIÃO CRÍTICA
		}else if(lastY + YCEIL < N){			//SE HOUVER COLUNAS,
													//VAMO LÁ
			m -> index_x = 0;					
			lastX = 0;
			m -> index_y = lastY + YCEIL;
			lastY += YCEIL;
			pthread_mutex_unlock(&mutex);		//SAI DA REGIÃO CRÍTICA
		}else{									//PROCESSOU TUDO QUE TINHA
			pthread_mutex_unlock(&mutex);		//SÓ SAI
			break;
		}
	}
	//cout << "===============\n\t" << l << "\n===============\n";
	pthread_exit(NULL);							//ENCERRA A THREAD
}

int main(){
	srand(SEED);					//SEMENTE

	//VARIÁVEIS
	long int NUM_THREADS = 2;			//NÚMERO DE THREADS
	int rc, t = 0;					//AUXILIARES PARA CRIAÇÃO DAS THREADS
										//ID DA THREAD E RETORNO DA FUNÇÃO
										//DE CRIAÇÃO

	int t1 = 0, t2 = 0;				//TEMPO

	pthread_t threads[NUM_THREADS];	//VETOR DE THREADS
	macrobloco m[NUM_THREADS];		//VETOR COM OS PARÂMETROS PASSADOS ÀS THREADS

	matrix = allocMatrix(N);		//ALOCAÇÃO
	fillMatrix(matrix, N);				//E PREENCHIMENTO DA MATRIZ
	
	//printMatrix(matrix);			//SÓ PRA VISUALIZAR/DEPURAR MATRIZES MENORES 
	

	pthread_mutex_init(&mutex_counter, NULL);	//INICIALIZAÇÃO DO MUTEX DO CONTADOR
	pthread_mutex_init(&mutex, NULL);	//INICIALIZAÇÃO DO MUTEX DOS PISOS

	t1 = time(NULL);					//INICIO DA CONTAGEM DO CALCULO SERIAL

	int x = countPrimesSerial(matrix);	//CALCULO SERIAL
	t2 = time(NULL) - t1;				//FIM DA CONTAGEM
	
	cout << "SERIAL:\n\t" << x << " números primos encontrados.\n\t" 
		 << time(NULL) - t1 << " segundos de processamento.\n";

	t1 = time(NULL);					//INICIO DA CONTAGEM CALCULO PARALELO	
	
	for(int i = 0; i < NUM_THREADS; i++, t++){
		lastY = m[t].index_y = 0;		//ULTIMOS ÍNDICES PROCESSADOS
		lastX = m[t].index_x = i*XCEIL;

		m[t].thread_id = t;				//ID DA THREAD PRA FINS DE DEPURAÇÃO
			
		rc = pthread_create(&threads[t], NULL, work, (void *)&m[t]);
		
		if(rc){
			cout << "Erro " << rc << "\n";
			exit(-1);			
		}
	}

	for(int i = 0; i < NUM_THREADS; i++)
		(void) pthread_join(threads[i], NULL);	//AGUARDA O PROCESSAMENTO DAS 
													//THREADS.

	t2 = time(NULL) - t1;				//FIM DA CONTAGEM CÁLCULO PARALELO

	cout << "PARALELA:\n\t" << count << " números primos encontrados.\n\t" << t2<< " segundos de processamento.\n";


	deallocMatrix(&matrix, N);			//DESALOCANDO MATRIZ
	pthread_exit(NULL);			
}
