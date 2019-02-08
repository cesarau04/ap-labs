#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define ERROR_CODE 1
#define SUCCESS_CODE 0
#define MAX_SIZE 128
void printUsage();

int main(int argc, char **argv)
{
	int fd, fd2;

	if (argc == 2) {
		char buff[MAX_SIZE];
		fd = open(argv[1], O_RDONLY);
		if (fd == -1) {
			printf("Couldn't open file\n");
			return ERROR_CODE;
		}

		while (read(fd, buff, MAX_SIZE - 1) != 0) {
			buff[MAX_SIZE] = '\0';
			printf("%s", buff);
		}
		close(fd);
	} else if (argc == 4) {
		if (strcmp(argv[2], "-c") == 0) {
			char *buff;
			buff = calloc(MAX_SIZE, sizeof(char)); 
			fd = open(argv[1], O_RDONLY);
			fd2 = open(argv[3], O_WRONLY | O_CREAT, S_IRWXU);
			if (fd == -1){
				printf("Couldn't open %s", argv[1]);
				close(fd);
				printUsage();
				return ERROR_CODE;
			}
			while (read(fd, buff, MAX_SIZE -1) != 0){
				write(fd2, buff, MAX_SIZE-1);
			}
			close(fd);
			close(fd2);
		}
	} else {
		printUsage();
	}
	return SUCCESS_CODE;
}

void printUsage()
{
	printf("./mycat file [FLAGS] [newfile]\n\nFLAGS:\n"
	       "  \t\t--Read and print to terminal. [DEFAULT]\n"
	       "-c\t\t--Copy content to a new file.\n");
}
