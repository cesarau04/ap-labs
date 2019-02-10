#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void usage();

int main(int argc, char **argv){
	if (argc <= 1){
		usage();
	}

	size_t i;
	size_t msglen = 0;
	for (i = 1; i < argc; i++){
		msglen += strlen(argv[i])+1; 
	} 
	char *msg;
	msg = calloc(msglen, sizeof(char));
	msg[0] = '\0';
	for (i = 1; i < argc; i++){
		strcat(msg, argv[i]);
		strcat(msg, " ");
	}
	msg[msglen-1] = '\n';

	DIR *dir; 
	struct dirent *pDirent;
	static char *path = "/dev/pts/";
	int errnum; 

	const int pathlength = strlen(path);

	dir = opendir(path);
	
	if (dir == NULL){
		perror("Error directory: ");
		return EXIT_FAILURE;
	}

	char *pathpts;
	size_t c;
	int flag = 0;
	int len, fd;
	i = 0;
	while ((pDirent = readdir(dir)) != NULL){
		len = strlen(pDirent->d_name);
		
		for (c = pDirent->d_name[i]; i<len; i++){
			if (isdigit(c) != 0){
				continue;
			}else{
				flag = -1;
				break;
			}
		}
		if (flag == -1){
			flag = 0;
			continue;
		}
		
		if ((pathpts = calloc((strlen(pDirent->d_name)+pathlength) + 1, sizeof(char))) == NULL){
			perror("Error calloc: ");
		}else{
			pathpts[0] = '\0';
			strcat(pathpts, path);
			strcat(pathpts, pDirent->d_name);
		}
		
		fd = open(pathpts, O_WRONLY | O_NOCTTY);
		if (fd == -1){
			perror("Error file descriptor: ");
		}
		write(fd, msg, msglen);
		close(fd);
	}

	closedir(dir);
	return EXIT_SUCCESS;
}

void usage(){
	printf(
			"Send a message to all pts terminals connected in the computer.\n\n"
			"Usage: ./broadcast [message]\n");
}
