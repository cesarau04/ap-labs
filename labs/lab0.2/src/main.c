#include <stdio.h>

int strlen(char *str);
char *stradd(char *origin, char *addition);
int strfind(char *origin, char *substr);

int main(int argc, char **argv)
{
	int result = strlen(&*argv[1]);
	printf("Lenght: %d\n", result);
	stradd(&*argv[1], &*argv[2]);
	printf("Concatenation: %s\n", argv[1]);
	printf("Substring found -> ");
	if (strfind(&*argv[1], &*argv[3])) {
		printf("True\n");
	} else {
		printf("False\n");
	}
}
