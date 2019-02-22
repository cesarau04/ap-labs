#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUFF_SIZE 512

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: ./cross-ref [file.txt]\n");
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Error opening file");
	}

	char *buff;
	buff = (char *)calloc(BUFF_SIZE, sizeof(char));

	char *line;
	line = (char *)calloc(BUFF_SIZE, sizeof(char));

	char *token;

	char separator[2] = " ";
	off_t offset = 0;
	while (read(fd, buff, BUFF_SIZE - 1)) {
		// get line
		int i;
		for (i = 0; buff[i] != '\n'; ++i) {
			line[i] = buff[i];
		}
		line[i] = '\0';
		offset += (i + 1);
		lseek(fd, offset, SEEK_SET);

		token = strtok(line, separator);

		while (token != NULL) {
			i = strlen(token);
			if (token[i-1] == '.' || token[i-1] == ',' || token[i-1] == ':' || token[i-1] == ';' || token[i-1] == ')' ){
				token[i-1] = '\0';
			}
			printf("%s ", token);
			token = strtok(NULL, separator);
		}
		printf("\n");
	}

	return 0;
}





