#include <ctime>
#include <cstdlib>
#include <iostream>
#include "utils.h"

bool primeNumber(unsigned num){
	if(num > 1){
		for(int i = 2; i*i <= num; i++)
			if(!(num%i))
				return false;
		
		return true;
	}
	return false;
}

unsigned **allocMatrix(unsigned size){
	unsigned **matrix;
	matrix = new unsigned*[size];
	for(int i = 0; i < size; i++)
		matrix[i] = new unsigned[size];
	
	return matrix;
}

void fillMatrix(unsigned **matrix, unsigned size){
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			matrix[i][j] = rand()%10;
}

void deallocMatrix(unsigned ***matrix, unsigned size){
	unsigned **matrix_ = *matrix;

	for(int i = 0; i < size; i++){
		std::cout << "\ndeletando a posicao..." << i;	
		delete []matrix_[i];
	}
	delete []matrix_;
}
