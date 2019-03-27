#define	 _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


#define PROC_PATH     "/proc/"
#define NAMEFILE_SIZE 25
#define STATE_SIZE    12

void clear();
bool isDirectory(unsigned char *type);
bool hasPidFormat(char *filename);
struct process *talloc(void);


struct process {
  unsigned int pid;
  unsigned int ppid;
  char*  p_name;
  char*  state;
  unsigned int memory;
  unsigned int no_threads;
  unsigned int no_openfd; 
  struct process *left;
  struct process *right;
};

int main(){
	struct process *root;
	DIR *dir;
	struct dirent *pDirent;

	root = NULL;
	dir = opendir(PROC_PATH);
	if (dir == NULL){
		perror("opendir(); line 26");
		exit(EXIT_FAILURE);
	}
	while((pDirent = readdir(dir)) != NULL){
		if (! (isDirectory(&pDirent->d_type) && hasPidFormat(pDirent->d_name)))
			continue;
		// root = addnode(root, )	
	}
  clear();
  return 0;
}

void clear() {
  printf("\e[1;1H\e[2J"); 
}

bool isDirectory(unsigned char *type){
	if (*type == DT_DIR)
		return true;
	return false;
}

bool hasPidFormat(char *filename){
	unsigned int len = strlen(filename);
	for (unsigned int i = 0; i < len; i++){
		if (isdigit(filename[i]) == 0)
			return false;
	}
	return true;
}

struct process *talloc(void)
{
	return (struct process *)malloc(sizeof(struct process));
}

struct process *addnode(struct process *p, unsigned int *pid)
{
	int cond;
	char ppidstr[12], pidstr[12];
	sprintf(ppidstr, "%d", p->pid);
	sprintf(pidstr, "%d", *pid);

	if (p == NULL) {
		p = talloc();
		p->pid = *pid;
		p->p_name = (char *) malloc(NAMEFILE_SIZE * sizeof(char));
		p->state  = (char *) malloc(STATE_SIZE    * sizeof(char));
		p->left = p->right = NULL;
	} else if ((cond = strcmp(pidstr, ppidstr)) == 0) {
		/*p->count++;*/
		/*strcat(p->lines, l);*/
	} else if (cond < 0) {
		/*p->left = addnode(p->left, w, l);*/
	} else {
		/*p->right = addnode(p->right, w, l);*/
	}
	return p;
}



