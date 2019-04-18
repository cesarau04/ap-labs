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

int NUM_BUFFERS; 
long **buffers; 
pthread_mutex_t *mutexes;
long *result; 

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

	long *matrixA, *matrixB;
	matrixA = readMatrix("matA.dat");
	matrixB = readMatrix("matB.dat");

	result = multiply(matrixA, matrixB);
	saveResultMatrix(result);

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
		panicf("row must be 0>row>2000\n");
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
	// HERE COMES THE MAGIC
	return NULL;
}

int saveResultMatrix(long *result){ int fd;
	fd = open("result.dat", O_WRONLY | O_CREAT);
	if (fd == -1){
		errorf("Couldn't create result.dat\n");
		return -1;
	}
	for (int i = 0; i < 4000000; i++){
		char buff[255];
		sprintf(buff, "%ld\n", result[i]);
		write(fd, buff, 255);
	}
	if (close(fd) != 0){
		perror("error closing fd: ");
		return -1;
	}
	return 0;
}


