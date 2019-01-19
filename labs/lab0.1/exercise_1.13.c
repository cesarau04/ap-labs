#include <stdio.h>
#include <string.h>

int len_word,i,j;
int lengths[25];
char string[25] = "";
char currentChar[3] = "";

void main(){
	/*for (i = 0; i<=25; i++){
		lengths[i] = 0;
	}*/

	int c;
	while((c=getchar())!=EOF){
		if(c == ' ' ||  c == '\t' || c == '\n' || c == ',' || c == '.' ){
			/* printf("----------\n");
			printf("word: %s\n", string);
			printf("length = %d\n", len_word);
			putchar('\n'); */
			len_word = strlen(string);
			++lengths[len_word]; 
			memset(string, 0, strlen(string));
			while((c=getchar()) == ' ' || c == '\t' || c == '\n' || c == ',' || c == '.');
		}
		snprintf(currentChar, 4, "%c", c);
		strcat(string, currentChar);
	}

	for (i = 1; i < 25; i++){
		printf("%2.0d:", i);
		printf("(%3d)  ", lengths[i]);
		for (j = 0; j<lengths[i]; j++){
			printf("|");
		}
		printf("\n");
	}
}
