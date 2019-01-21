#include <stdio.h>
#include <string.h>

#define MAXLINE 1000
#define INCOMMENT 1
#define OUTCOMMENT 0

int getline(char line[], int maxline);
void copy(char to[], char from[]);

int status = OUTCOMMENT;

int main(){
    int len;
    int max;
    char line[MAXLINE];
    /* char longest[MAXLINE];*/

    max = 80;
    while ((len = getline(line, MAXLINE)) > 0){
        printf("%s\n", line);
        memset(line, 0, strlen(line)); /* clear lasttwochars*/
    }
    return 0;
}

int getline(char s[], int lim){
    int c, i, balance, index;
    balance = 0;
    char begincomment[3]    = "/*";
    char endcomment[3]      = "*/";
    char lasttwochars[3]    = "  ";
    char tmp[3];
    
    c = 0;
    index = 0;
    while(c != EOF){
        if (c == '\n')
            break;
        for (i=0; i<lim-1 && (c=getchar()) !=EOF && c != '\n'; ++i){
            if (status != INCOMMENT){
                s[index] = c;
                ++index;
            }
    
            tmp[0] = lasttwochars[1];   /* get previous chr */
            tmp[1] = c;                 /* get last readed chr*/
            tmp[2] = '\0';              /* create string */
            memset(lasttwochars, 0, strlen(lasttwochars)); /* clear lasttwochars*/
            strcpy(lasttwochars, tmp);
            if (strcmp(lasttwochars, begincomment) == 0){
                status = INCOMMENT;
                s[index-1] = 0;
                s[index-2] = 0;
                if (i<=index){
                    index = i-1;
                }
                ++balance;
                lasttwochars[0] = '*';
                lasttwochars[1] = ' ';
                lasttwochars[2] = '\0';
            }else if(strcmp(lasttwochars, endcomment) == 0){
                --balance;
                lasttwochars[0] = '/';
                lasttwochars[1] = ' ';
                lasttwochars[2] = '\0';
                if(balance == 0){
                    status = OUTCOMMENT;
                }
            }
        }
    }

    if (c == '\n') {
        s[index] = c;
        ++i;
    }
    s[index] = '\0';
    return index;
}
