#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define BUFF_SIZE 218
#define LINES_SIZE 512
#define INT_TO_STR_SIZE 13
#define BANNED_WORDS 80
#define WORD_SIZE 100

char bannedwords[BANNED_WORDS][WORD_SIZE];
struct tnode {
	char *word;
	int count;
	char *lines;
	struct tnode *left;
	struct tnode *right;
};

struct tnode *addnode(struct tnode *p, char *w, char *l);
void treeprint(struct tnode *p);
void cleantoken(char *str);
struct tnode *talloc(void);
char *strdup2 (char *str);
int isfamousword(char *t);

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: ./cross-ref [file.txt]\n");
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Error opening file");
		close(fd);
		return 1;
	}

	int fd2 = open("wordsban.txt", O_RDONLY);
	if (fd == -1) {
		perror("Couldn't open wordsban.txt");
		close(fd2);
		return 1;
	}

	struct tnode *root;	
	root = NULL;

	char *buff;
	buff = (char *)calloc(BUFF_SIZE, sizeof(char));

	char *line;
	line = (char *)calloc(BUFF_SIZE, sizeof(char));

	char *linenumberstr;
	linenumberstr = (char *)calloc(INT_TO_STR_SIZE, sizeof(char));

	char *token;
	char separator[2] = " ";
	off_t offset = 0;
	int linenumber = 1;
	
	int indexarray = 0;
	while (read(fd2, buff, BUFF_SIZE - 1)) {
		int i;
		for (i = 0; buff[i] != '\n'; ++i) {
			line[i] = buff[i];
		}
		line[i] = '\0';
		offset += (i + 1);
		lseek(fd2, offset, SEEK_SET);
		strcpy(bannedwords[indexarray], line);
		++indexarray;
	}
	close(fd2);

	offset = 0;
	while (read(fd, buff, BUFF_SIZE - 1)) {
		int i;
		for (i = 0; buff[i] != '\n'; ++i) {
			line[i] = buff[i];
		}
		line[i] = '\0';
		offset += (i + 1);
		lseek(fd, offset, SEEK_SET);

		token = strtok(line, separator);
		while (token != NULL) {
			cleantoken(token);
			if (isalpha(token[0]) != 0){
				if (isfamousword(token) == 0){	
					sprintf(linenumberstr, "%d,", linenumber);
					root = addnode(root, token, linenumberstr);
				}
			}
			token = strtok(NULL, separator);
		}
		++linenumber;
	}
	close(fd);
	printf("%s  %-30s\t %s\n", "COUNT", "WORD", "LINES");
	printf("------------------------------------------------------------------------------------------\n");
	treeprint(root);
	return 0;
}


struct tnode *addnode(struct tnode *p, char *w, char *l){
	int cond;
	
	if (p == NULL){
		p = talloc();
		p->word = strdup2(w);
		p->count = 1;
		p->lines = malloc(LINES_SIZE);
		p->lines[0] = '\0';
		strcpy(p->lines, l);
		p->left = p->right = NULL;
	}else if ((cond = strcmp(w, p->word)) == 0){
		p->count++;
		strcat(p->lines, l);	
	}else if (cond < 0) {
		p->left = addnode(p->left, w, l);
	}else {
		p->right = addnode(p->right, w, l);
	}
	return p;
}

void treeprint(struct tnode *p)
{
	if (p != NULL) {
		treeprint(p->left);
		printf("%5d: %-30s\t %s\n", p->count, p->word, p->lines);
		treeprint(p->right);
	}
}

struct tnode *talloc(void){
	return (struct tnode *) malloc(sizeof(struct tnode));
}

char *strdup2(char *str)
{
	char *p;
	p = (char *) malloc(strlen(str)+1); 
	if (p != NULL)
		strcpy(p, str);
	return p;
}

int isfamousword(char *t){
	char *tcopy;
	tcopy = (char *)malloc(WORD_SIZE);
	size_t j;
	for (int i = 0; i < BANNED_WORDS; ++i){
		for (j = 0; j < strlen(t); ++j){
			tcopy[j] = toupper(t[j]);
		}
		tcopy[j] = '\0';
		if (strcmp(bannedwords[i], tcopy) == 0)
			return 1;
	}
	return 0;
}
void cleantoken(char *str)
{
	int index;
	index = 0;
	while (str[index] == '*' || str[index] == '"' || str[index] == '(' ||
	       str[index] == '-') {
		++index;
	}
	int i = 0;
	if (index > 0) {
		while (str[i + index] != '\0') {
			str[i] = str[i + index];
			++i;
		}
		str[i] = '\0';
		for (i = (i - 1); isalpha(str[i]) == 0 && i>-1; --i){
			str[i] = '\0';
		}
	} else {
		for (i = strlen(str) - 1; isalpha(str[i]) == 0 && i>-1; --i){
			str[i] = '\0';
		}
	}
}
