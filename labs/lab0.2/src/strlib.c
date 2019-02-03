/* 
 * Library to get strlen, adding two strings, and finding a substring.
*/
#include <stddef.h>

char *strslice(char *origin, size_t upto)
{
	size_t i;
	char *rtn;
	rtn = (char *)malloc(upto * sizeof(char));
	for (i = 0; i < upto; i++) {
		rtn[i] = origin[i];
	}
	return rtn;
}

int strlen(char *str)
{
	int counter = 0;
	while (*str != '\0') {
		++counter;
		++str;
	}
	return counter;
}

char *stradd(char *origin, char *addition)
{
	size_t i, j;
	i = strlen(&*origin);
	for (j = 0; addition[j] != '\0'; j++)
		origin[i + j] = addition[j];
	origin[i + j] = '\0';
	return origin;
}

int strfind(char *origin, char *substr)
{
	size_t i, x, y, lim;
	x = strlen(&*origin);
	y = strlen(&*substr);
	if (y > x) {
		return 0;
	}
	if (y == 0) {
		return 1;
	}
	lim = x - y;
	for (i = 0; i <= lim; i++) {
		if (origin[i] == substr[0]) {
			if (strfind
			    (strslice(&origin[i + 1], (y - 1)), &substr[1])) {
				return 1;
			}
			continue;
		} else {
		}
	}
	return 0;
}
