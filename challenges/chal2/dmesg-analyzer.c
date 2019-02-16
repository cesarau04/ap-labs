#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

#define REPORT_FILE "report.txt"

void analizeLog(char *logFile, char *report);
void gettype(char *type, size_t position, char *line);
int checkarray(char *type);

struct logType {
	char *type;
	char *description;
};

//Upto 2048 different types 
struct logType arrayLogTypes[2048];
size_t ARRAY_INDEX = 1;
size_t DESCRIPTION_SIZE = 8096;

int main(int argc, char **argv)
{

	if (argc < 2) {
		printf("Usage:./dmesg-analizer.o logfile.txt\n");
		return 1;
	}

	analizeLog(argv[1], REPORT_FILE);

	return 0;
}

void analizeLog(char *logFile, char *report)
{
	printf("Generating Report from: [%s] log file\n", logFile);

	int fd;
	fd = open(logFile, O_RDONLY);
	if (fd == -1) {
		perror("");
	}

	size_t BUFF_SIZE = 512; 
	size_t LINE_SIZE = 256;
	size_t TYPE_SIZE = 64;

	struct logType general;
	general.type = calloc(TYPE_SIZE, sizeof(char));
	general.description = calloc(DESCRIPTION_SIZE, sizeof(char));
	strcpy(general.type,"General");
	strcpy(general.description, "");

	arrayLogTypes[0] = general;

	char *buff;
	buff = calloc(BUFF_SIZE, sizeof(char));

	char *line;
	line = calloc(LINE_SIZE, sizeof(char));

	char *type;
	type = calloc(TYPE_SIZE, sizeof(char));

	char *prevline;
	prevline = calloc(LINE_SIZE, sizeof(char));

	char *prevtype;
	prevtype = calloc(TYPE_SIZE, sizeof(char));

	bool expand_description = false;
	bool description_saved = false;

	off_t seekcounter = 0;
	while ((read(fd, buff, BUFF_SIZE - 1) > 0)) {
		memset(line, '\0', LINE_SIZE);
		memset(type, '\0', TYPE_SIZE);

		size_t i;
		for (i = 0; i < BUFF_SIZE; ++i) {
			line[i] = buff[i];
			if (buff[i] == '\n') {
				break;
			}
		}
		seekcounter += (i + 1);
		memset(buff, '\0', BUFF_SIZE);
		lseek(fd, seekcounter, SEEK_SET);
		// At this point line is already, buff is clear, and seek is set correctly.
		
		if (expand_description){
			char *pLine = line;
			size_t j;
			for (j = 0; j < 15; ++pLine && ++j){}
			if (pLine[0] == ' '){ //found expand_description style
				//1) check for previous type, if it exits append else create new struct.
				int id = checkarray(prevtype);
				if (id == -1){ //no exists	
					struct logType tmpStruct;
					tmpStruct.type = calloc(TYPE_SIZE, sizeof(char));
					tmpStruct.description = calloc(DESCRIPTION_SIZE, sizeof(char));	
					strcpy(tmpStruct.type, prevtype);
					strcpy(tmpStruct.description, "\t");
					strcat(tmpStruct.description, line);
					arrayLogTypes[ARRAY_INDEX++] = tmpStruct;
					continue;
				}else{	
					strcat(arrayLogTypes[id].description, "\n\t");
					strcat(arrayLogTypes[id].description, line);
				}
				expand_description = true;
				description_saved = true;
			}else{
				/*
				if (!description_saved){				
					//1) save last line to general			
					strcat(arrayLogTypes[0].description, "\t");
					strcat(arrayLogTypes[0].description, prevline);
					strcat(arrayLogTypes[0].description, "\n");
					memset(prevline, '\0', LINE_SIZE);
					memset(prevtype, '\0', TYPE_SIZE);
					expand_description = false;
				}*/
			}
		}
		// Check if there is  POSSIBLE description next line's
		if (line[i-1] == ':'){
			//1) get posible type
			gettype(type, i-1, line);
			//2) update variable for this special case.
			strcpy(prevline, line);
			strcpy(prevtype, type);
			expand_description = true;
			continue;
		}

		// Check if there is : anywhere, but not follow by something.
		int hastype = 0;
		size_t j;
		for (j = 0; j < i; ++j){
			if (line[j] == ':'){
				if (line[j+1] == ' '){
					hastype = 1;
					break;
				}
			}
		}
		
		if(hastype){
			//1) get type
			gettype(type, j, line);
			//2) look for types inserted
			int id = checkarray(type);
			if(id == -1){ //create if does not exists
				struct logType tmpStruct;
				tmpStruct.type = calloc(TYPE_SIZE, sizeof(char));
				tmpStruct.description = calloc(DESCRIPTION_SIZE, sizeof(char));	
				strcpy(tmpStruct.type, type);
				strcpy(tmpStruct.description, "\t");
				strcat(tmpStruct.description, line);
				arrayLogTypes[ARRAY_INDEX++] = tmpStruct; 
			}else{ //append to existing one
				strcat(arrayLogTypes[id].description, "\n\t");
				strcat(arrayLogTypes[id].description, line);
			}
		}else{
			// go to general
			// update struct general
			if(description_saved){
				description_saved = false;
			}else{
			strcat(arrayLogTypes[0].description, "\t");
			strcat(arrayLogTypes[0].description, line);
			strcat(arrayLogTypes[0].description, "\n");
			}
		}

	}
	close(fd);
	fd = open(report, O_WRONLY | O_CREAT, 0644);
	for(size_t i = 0; i < ARRAY_INDEX; ++i){
		write(fd, arrayLogTypes[i].type, strlen(arrayLogTypes[i].type));
		write(fd, "\n", 1);
		write(fd, arrayLogTypes[i].description, strlen(arrayLogTypes[i].description));
		write(fd, "\n", 1);
		
	}
	close(fd);
	printf("\nReport is generated at: [%s]\n", report);
	
}

/* type->String to write the result
 * position->Position of the ':'
 * line->Line to pull the data from */
void gettype(char *type, size_t position, char *line)
{
	char *pLine = line;
	size_t j;
	for (j = 0; j < 15; ++pLine && ++j){}
	for (j = 0; j < position-15; ++j){
		type[j] = pLine[j];
	}
	type[j] = '\0';
}

/* type->String of type to check
 * RETURN: the index of the matching struct, on error -1. */
int checkarray(char *type){
	for (size_t i = 1; i < ARRAY_INDEX; ++i){
		if (strcmp(arrayLogTypes[i].type, type) == 0){
			return i;
		}
	}
	return -1;
}
