#include <stdio.h>

#define MAXLINE 1000

int getline(char line[], int maxline);
void copy(char to[], char from[]);

int main(){
    int len;
    int max;
    char line[MAXLINE];
    char longest[MAXLINE];

    max = 0;
    while ((len = getline(line, MAXLINE)) > 0)
        if (len > max) {
            max = len;
            copy(longest, line);
    }
    if (max> 0)
        printf("Termina con: %s", longest);
        printf("Tamaño: %d\n", max);
    return 0;
}

int getline(char s[], int lim){
    int c, i, len_counter;
    
    c = 0;
    len_counter = 0;
    while(c != EOF){
        if (c == '\n')
            break;
        for (i=0; i<lim-1 && (c=getchar()) !=EOF && c != '\n'; ++i){
            s[i] = c;
            ++len_counter;
        }
    }
    if (c == '\n') {
        s[i] = c;
        ++i;
    }
    s[i] = '\0';
    return len_counter;
}        

void copy(char to[], char from[]){
    int i;
    i = 0;
    while ((to[i] = from[i]) != '\0')
        ++i;
}
