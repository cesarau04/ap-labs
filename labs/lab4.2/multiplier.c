#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "logger.h"

long * readMatrix(char *filename);
long * getColumn(int col, long *matrix);
long * getRow(int row, long *matrix);
int getLock(void);
int releaseLock(int lock);
long dotProduct(long *vec1, long *vec2);
long * multiply(long *matA, long *matB);
int saveResultMatrix(long *result);
void *threadFunc(void *arg);

int NUM_BUFFERS; 
long **buffers; 
pthread_mutex_t *mutexes;
long *result; 
pthread_t threads[2000];

struct DataStruct{
	long *matA;
	long *matB;
	size_t rowPosition;
	size_t colPosition;
	long *result;
};

int main(int argc, char **argv)
{
	if (argc != 3){
		errorf("USAGE: ./multiplier -n NUMBER\n");
		exit(EXIT_FAILURE);
	}
	NUM_BUFFERS = strtol(argv[2], NULL, 10);
	infof("NUM_BUFFERS => %d\n", NUM_BUFFERS);
	
	// TODO: allocate inside buffers
	buffers = (long **) malloc(NUM_BUFFERS * sizeof(long*));
	mutexes = (pthread_mutex_t *) malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));

	// TODO: Need to check if this works 
	for (int i = 0; i < NUM_BUFFERS; i++){
		pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
		mutexes[i] = mutex;
		pthread_mutex_init(&mutexes[i], NULL);
	}

	infof("pthreads created\n");

	long *matrixA, *matrixB;
	matrixA = readMatrix("matA.dat");
	matrixB = readMatrix("matB.dat");
	
	infof("matrices read\n");

	result = multiply(matrixA, matrixB);
	infof("saving result...\n");
	saveResultMatrix(result);
	infof("result saved\n");

	free(matrixA);
	free(matrixB);
	free(buffers);
	free(mutexes);
	free(result);
	return 0;
}

long * readMatrix(char *filename){
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	fp = fopen(filename, "r");
	if (fp == NULL){
		panicf("Couldn't open %s\n", filename);
		return NULL;
	}
	long *matrix;
	matrix = (long *) malloc(4000000 * sizeof(long));
	int i = 0;
	while (getline(&line, &len, fp) != -1){
		matrix[i++] = strtol(line, NULL, 10);
	}
	free(line);
	return matrix;
}

long * getColumn(int col, long *matrix){
	if (col < 0 || col > 2000){
		panicf("col must be 0>col>2000\n");
		exit(EXIT_FAILURE);
	}
	size_t offset = col-1;
	long *mycol;
	mycol = (long *) malloc(2000 * sizeof(long));

	for (int i = 0; i < 2000; i++){
		mycol[i] = matrix[offset];
		offset += 2000;
	}
	return mycol;
}

long * getRow(int row, long *matrix){
	if (row < 0 || row > 2000){
		panicf("row must bm 0>row>2000\n");
		exit(EXIT_FAILURE);
	}
	size_t offset = ((2 * row) - 2) * 1000;
	long *myrow;
	myrow = (long *) malloc(2000 * sizeof(long));
	
	for (int i = 0; i < 2000; i++){
		myrow[i] = matrix[offset++];
	}
	return myrow;
}

int getLock(void){
	for (int i = 0; i < NUM_BUFFERS; i++){
		if (pthread_mutex_trylock(&mutexes[i]) == 0)
			return i;
	}
	return -1;
}

int releaseLock(int lock){
	// TODO: Release the buffer	
	if (pthread_mutex_unlock(&mutexes[lock]) == 0){
		return 0;
	}
	return -1;
}

long dotProduct(long *vec1, long *vec2){
	long result = 0;
	for (int i = 0; i < 2000; i++)
			result += vec1[i]*vec2[i];
	return result;
}

long * multiply(long *matA, long *matB){
	result  = (long *) malloc(4000000 * sizeof(long));
	
	for (size_t i = 0; i < 2000; i++){
		// Create threads
		for (size_t j = 0; j < 2000; j++){
			struct DataStruct *currentStruct;
			currentStruct = (struct DataStruct *) malloc(sizeof(struct DataStruct));
			currentStruct->matA = matA;
			currentStruct->matB = matB;
			currentStruct->rowPosition = i + 1;
			currentStruct->colPosition = j + 1;
			currentStruct->result = result;
			pthread_create(&threads[j], NULL, threadFunc, (void *) currentStruct);
		}
		
		// Wait threads
		for (size_t j = 0; j < 2000; j++)
			pthread_join(threads[j], NULL);	
		printf("\r%ld%%", (i*100)/1999);
    fflush(stdout);
	}
	printf("\n");
	return result;
}

int saveResultMatrix(long *result){
	FILE *f = fopen("result.dat", "w");
	if (f == NULL)
	{
    errorf("error: couldn't open/create result.dat\n");
    return -1;
	}

	for (size_t i = 0; i < 3999999; i++)
		fprintf(f, "%ld\n", result[i]);

	fclose(f);
	return 0;
}

void *threadFunc(void *arg){
	struct DataStruct *data = (struct DataStruct *) arg;
	long *row, *col;
	long index;

	row = getRow(data->rowPosition, data->matA);
	col = getColumn(data->colPosition, data->matB);
	
	index = ((((data->rowPosition - 1) * 2000) + data->colPosition) - 1); 
	data->result[index] = dotProduct(row, col);

	free(row);
	free(col);
	free(arg);
	return NULL;
}




