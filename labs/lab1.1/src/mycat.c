#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define ERROR_CODE 1
#define SUCCESS_CODE 0
#define MAX_SIZE 128
void printUsage();

int main(int argc, char **argv){
	int fd;

	if (argc == 2){
		char buff[MAX_SIZE];
		fd = open(argv[1], O_RDONLY);
		if (fd == -1){
			printf("Couldn't open file\n");
			return ERROR_CODE;
		}

		while(read(fd, buff, MAX_SIZE-1) != 0){
			buff[MAX_SIZE] = '\0';
			printf("%s",buff);
		}
		printf("\n");
		close(fd);
	}else if (argc == 4) {
	
	}else{
		printUsage();	
	}

	/*
	if (strcmp(argv[2], "-c") == 0){
		printf("Copy\n");
	}else{
		printf("To terminal\n");
	}
	*/
	return SUCCESS_CODE;
}

void printUsage(){
	printf("./mycat file [FLAGS] [newfile]\n\nFLAGS:\n"
			"-r\t\t--Read and print to terminal. [DEFAULT]\n"
			"-c\t\t--Copy content to a new file.\n");
}
