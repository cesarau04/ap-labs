#define	 _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

#define PROC_PATH     "/proc/"
#define NAMEFILE_SIZE 25
#define STATE_SIZE    12

void clear();
bool isDirectory(unsigned char *type);
bool hasPidFormat(char *filename);
struct process *talloc(void);
struct process *addnode(struct process *p, unsigned int *pid);
unsigned int parseUnsigedInt(char * str);
void treeprint(struct process *p);
static void *findInfoForProcess(void *argv);

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

struct threadstruct {
	struct process *mynode;
	unsigned int pid;
};

size_t threadsopen = 0;

int main(){
	DIR *dir;
	struct process *root;
	struct dirent *pDirent;
	unsigned int param;

	root = NULL;
	dir = opendir(PROC_PATH);
	param = 0;
	if (dir == NULL){
		perror("opendir(); line 26");
		exit(EXIT_FAILURE);
	}
	
	while((pDirent = readdir(dir)) != NULL){
		if (! (isDirectory(&pDirent->d_type) && hasPidFormat(pDirent->d_name)))
			continue;
		param = parseUnsigedInt(pDirent->d_name);
		root = addnode(root, &param);	
	}
	treeprint(root);
  //clear();
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

	if (p == NULL) {
		p = talloc();
		p->pid = *pid;
		p->p_name = (char *) malloc(NAMEFILE_SIZE * sizeof(char));
		p->state  = (char *) malloc(STATE_SIZE    * sizeof(char));
		p->left = p->right = NULL;
		// aqui llamada a thread.
	} else if ((cond = (*pid > p->pid) ? -1 : 1 ) < 0) {
		p->left = addnode(p->left, pid);
	} else {
		p->right = addnode(p->right, pid);
	}
	return p;
}

unsigned int parseUnsigedInt(char *st)
{
	char *x;
  for (x = st ; *x ; x++) {
    if (!isdigit(*x))
      return 0L;
  }
  return (strtoul(st, 0L, 10));
}

void treeprint(struct process *p)
{
	if (p != NULL) {
		treeprint(p->right);
		printf("pod:%u\n", p->pid);
		treeprint(p->left);
	}
}

static void *findInfoForProcess(void *argv){

}
