#include <stdio.h>
#include <ctype.h>

int i,j;
int alphabet[26];

void main(){
	int c;
	while((c=getchar())!=EOF){
		if(c == ' ' ||  c == '\t' || c == '\n' || c == ',' || c == '.' ){
			while((c=getchar()) == ' ' || c == '\t' || c == '\n' || c == ',' || c == '.');
			if (c == EOF)
				break;
		}
		c = toupper(c);
		if (c < 0 || c > 25 )
		++alphabet[c-65];
	}

	for (i = 0; i < 26; i++){
		printf("%c:", (i+65));
		printf("(%3d)  ", alphabet[i]);
		for (j = 0; j<alphabet[i]; j++){
			printf("|");
		}
		printf("\n");
	}
}
