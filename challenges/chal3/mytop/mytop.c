#define	 _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PROC_PATH     "/proc/"
#define FD_PATH       "/fd/"    //PROC_PATH + PID + FD_PATH
#define MEMORY_PATH   "/statm"  //PROC_PATH + PID + MEMORY_PATH
#define STATUS_PATH   "/status" 
#define NAMEFILE_SIZE 25
#define STATE_SIZE    12
#define MAX_PATH_SIZE 25
#define INT_TO_STR_SIZE 11
#define BUFF_SIZE     512


void clear();
bool isDirectory(unsigned char *type);
bool hasPidFormat(char *filename);
struct process *talloc(void);
struct threadstruct *threadalloc(void);
struct process *addnode(struct process *p, unsigned int *pid);
unsigned int parseUnsigedInt(char * str);
void treeprint(struct process *p);
void *findInfoForProcess(void *argv);
unsigned int countFds(char *path);

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

int threadsopen = 0;
pthread_t threadslist[1000];

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

	closedir(dir);

	for(int i = 0; i < threadsopen; i++){
		pthread_join(threadslist[i], NULL);	
	}
	//treeprint(root);
	//clear();
  return 0;
}

void clear() {
  printf("\e7"); 
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

struct threadstruct *threadalloc(void)
{
	return (struct threadstruct *)malloc(sizeof(struct threadstruct));
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

		struct threadstruct *ts;
		ts = threadalloc();
		ts->mynode = p;
		ts->pid    = *pid;
		int s = pthread_create(&threadslist[threadsopen++], NULL, findInfoForProcess, (void *) ts);
		if (s != 0){
			printf("Couldn't create thread for: %u", *pid);
			exit(EXIT_FAILURE);
		}
		threadsopen++;
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

void *findInfoForProcess(void *param){
	struct threadstruct* ts;
	ts = (struct threadstruct *) param;
	/* Save the routes for the files */
	char *mempath, *fdpath, *statuspath, *pidstr, *buff, *dataph;
	mempath    = (char *) malloc(MAX_PATH_SIZE * sizeof(char));
	statuspath = (char *) malloc(MAX_PATH_SIZE * sizeof(char));
	fdpath     = (char *) malloc(MAX_PATH_SIZE * sizeof(char));
	pidstr     = (char *) malloc(INT_TO_STR_SIZE * sizeof(char));
	buff       = (char *) malloc(BUFF_SIZE * sizeof(char));
	dataph     = (char *) malloc(NAMEFILE_SIZE * sizeof(char));  //use the largest posible value to store

	sprintf(pidstr, "%u", ts->mynode->pid);
	strcpy(mempath, PROC_PATH);
	strcat(mempath, pidstr);     // /proc/<pid>
	strcpy(fdpath, mempath);      
	strcpy(statuspath, mempath);
	strcat(mempath, MEMORY_PATH);
	strcat(fdpath, FD_PATH);
	strcat(statuspath, STATUS_PATH);
	free(pidstr);	

	// Read statm, for memory usage
	int fd;
	fd = open(mempath, O_RDONLY);
	if (fd == -1){
		printf("Couldn't open mempath: %s", mempath);
		exit(EXIT_FAILURE);
	}
	// This one is fast, just read the until the first ' ' and save to memory utilization
	int i = 0;
	while (read(fd, buff, BUFF_SIZE) > 0){
		for (; buff[i] != ' '; i++){
			dataph[i] = buff[i];	
		}		
	}
	dataph[i] = '\0';
	ts->mynode->memory = parseUnsigedInt(dataph); 
	//free(mempath);
	close(fd);
	
	ts->mynode->no_openfd = countFds(fdpath);
	//free(fdpath);

	fd = open(statuspath, O_RDONLY);
	if (fd == -1){
		printf("Couldn't open statuspath: %s", statuspath);
		exit(EXIT_FAILURE);
	}
	
	i = 0;
	dataph[0] = '\0'; //Is even necessary?
	char *attr;
	attr = (char *) malloc(NAMEFILE_SIZE*sizeof(char));
	while (read(fd, buff, BUFF_SIZE) > 0) {
		for (; buff[i] != '\n'; i++){
			dataph[i] = buff[i];		
		}
		dataph[i] = '\0';
	}
	close(fd);
	
	return (void *) (size_t) ts->pid;
}

unsigned int countFds(char *path){
	unsigned int file_count = 0;
	DIR *dir;
	struct dirent *entry;

	dir = opendir(path); 
	if (dir == NULL){
		perror("opendir(); line 216");
		exit(EXIT_FAILURE);
	}
	while ((entry = readdir(dir)) != NULL)
         file_count++;
	closedir(dir);
	return file_count - 2;
}

