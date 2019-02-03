#include <stdio.h>

int strlen(char *str);

int main(int argc, char **argv)
{
	int result = strlen(&*argv[1]);
	printf("Lenght: %d\n", result);
	char *newstring;
	stradd(&*argv[1], &*argv[2]);
	printf("Concatenation: %s\n", argv[1]);
	printf("Substring found -> ");
	if (strfind(&*argv[1], &*argv[3])) {
		printf("True\n");
	} else {
		printf("False\n");
	}
}
