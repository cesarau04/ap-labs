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
#include <ctype.h>

#define PROC_PATH     "/proc/"
#define FD_PATH       "/fd/"    //PROC_PATH + PID + FD_PATH
#define MEMORY_PATH   "/statm"  //PROC_PATH + PID + MEMORY_PATH
#define STATUS_PATH   "/status" 
#define NAMEFILE_SIZE 256
#define STATE_SIZE    20
#define MAX_PATH_SIZE 50
#define INT_TO_STR_SIZE 11
#define BUFF_SIZE     2048
#define ATTR_SIZE     512
#define INFO_SIZE     2048


void clear();
bool isDirectory(unsigned char *type);
bool hasPidFormat(char *filename);
struct process *talloc(void);
struct threadstruct *threadalloc(void);
struct process *addnode(struct process *p, unsigned int *pid);
unsigned int parseUnsigedInt(char * str);
void treeprint(struct process *p);
void treeclean(struct process *p);
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
	while(true){
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
		printf("| %-11s | %-11s | %-45s | %-20s | %-11s | %-11s | %-11s |\n", 
				"PID", "PPID", "NAME", "STATUS", "MEMORY", "#THREADS", "OPENFILES");
		for (int u = 0; u < 143; u++){
			printf("-");
		}
		printf("-\n");
		treeprint(root);
		treeclean(root);
		threadsopen = 0;
		/*sleep(3);*/
	  clear();
	}
  return EXIT_SUCCESS;
}

void clear() {
  printf("\e[1;1H\e[;f\e[2J"); 
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
	} else if ((cond = *pid > p->pid ? -1 : 1 ) < 0) {
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
		printf("| %-11u | %-11u | %-45s | %-20s | %-11uMB | %-11u | %-11u |\n", 
			p->pid, p->ppid, p->p_name, p->state, (p->memory/1000), p->no_threads, p->no_openfd);
		treeprint(p->left);
	}
}

void treeclean(struct process *p)
{
	if (p != NULL) {
		treeclean(p->right);
		free(p);
		treeclean(p->left);
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
	dataph     = (char *) malloc(BUFF_SIZE * sizeof(char));  //use the largest posible value to store

	sprintf(pidstr, "%u", ts->mynode->pid);
	strcpy(mempath, PROC_PATH);
	strcat(mempath, pidstr);     // /proc/<pid>
	strcpy(fdpath, mempath);      
	strcpy(statuspath, mempath);
	strcat(mempath, MEMORY_PATH);
	strcat(fdpath, FD_PATH);
	strcat(statuspath, STATUS_PATH);
	free(pidstr);	

	int fd;
	fd = open(mempath, O_RDONLY);
	if (fd == -1){
		printf("Couldn't open mempath: %s", mempath);
		ts->mynode->memory = 0; 
	}

	int i = 0;
	while (read(fd, buff, BUFF_SIZE) > 0){
		for (; buff[i] != ' '; i++){
			dataph[i] = buff[i];	
		}		
	}
	dataph[i] = '\0';
	ts->mynode->memory = parseUnsigedInt(dataph); 
	free(mempath);
	close(fd);
	
	ts->mynode->no_openfd = countFds(fdpath);
	free(fdpath);

	fd = open(statuspath, O_RDONLY);
	if (fd == -1){
		printf("Couldn't open statuspath: %s", statuspath);
		exit(EXIT_FAILURE);
	}
	/*free(statuspath);*/
	char *attr, *info;
	attr = (char *) malloc(ATTR_SIZE*sizeof(char));
	info = (char *) malloc(INFO_SIZE*sizeof(char));
	int offset = 0;
	int opt;
	while (read(fd, buff, BUFF_SIZE) > 0) {
		i = 0;
		for (; buff[i] != '\n'; i++){
			dataph[i] = buff[i];	
		}
		dataph[i] = '\0';
		offset += ++i;
		lseek(fd, offset, SEEK_SET);
		int j;
		for (j = 0; dataph[j] != ':'; j++){
			attr[j] = dataph[j];
		}
		attr[j] = '\0';

		if (strcmp(attr, "Name") == 0){
			opt = 1;
		} else if (strcmp(attr, "State") == 0){
			opt = 2;
		} else if (strcmp(attr, "Threads") == 0){
			opt = 3;
		} else if (strcmp(attr, "PPid") == 0){
			opt = 4;
		} else {
			// memset(dataph, '\0', NAMEFILE_SIZE);
			lseek(fd, offset, SEEK_SET);
			continue;
		}

		int k = 0;
		for (j++; dataph[j] != '\0'; j++){
			if (isspace(dataph[j]) != 0){
				continue;
			}
			info[k++] = dataph[j];
		}
		info[k]='\0';

		switch (opt) {
			case 1:
				strcpy(ts->mynode->p_name, info);
				break;
			case 2:
				strcpy(ts->mynode->state, info);
				break;
			case 3:
				ts->mynode->no_threads = parseUnsigedInt(info);
				break;
			case 4:
				ts->mynode->ppid = parseUnsigedInt(info);
				break;
		}
		// memset(dataph, '\0', NAMEFILE_SIZE);

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

