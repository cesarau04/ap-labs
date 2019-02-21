#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 256

void usage();

int main(int argc, char **argv){
	if (argc != 2){
		usage();
		return 1;
	}

	int fd;
	fd = open(argv[1], O_RDONLY);
	if (fd == -1){
		perror("Ooops! Check your file");
		return 1;
	}
	char *buff;
	buff = (char *) calloc(LINE_SIZE, sizeof(char));

	char *line;
	line = (char *) calloc(LINE_SIZE, sizeof(char));

	off_t seekval = 0;
	int i;
	while (read(fd, buff, LINE_SIZE-1)){
		for (i = 0; buff[i] != '\n'; ++i){
			line[i] = buff[i];
		}
		line[i] = '\0';
		seekval += (i + 1);
		lseek(fd, seekval, SEEK_SET);
		memset(buff, '\0', LINE_SIZE);

		int j;
		int position = -1;
		for (j = 0; j < (i-1); ++j){
			if (line[j] == '/'){
				if (line[j-1] == '"'){
					continue;
				}
				if (line[j+1] == '/'){
					position = j;
					break;
				} else if (line[j+1] == '*'){
					position = j;
					break;
				}
			}
		}

		if (position == -1){
			printf("%s\n", line);
		} else {
			for (i = 0; i < position; ++i){
				printf("%c",line[i]);
			}
			printf("\n");
		}

		memset(line, '\0', LINE_SIZE);

	}
	close(fd);
	return 0;
}

void usage(){
	printf("Usage:  exercise_1.23.out [file.c]\n");
}
