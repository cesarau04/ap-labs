#include <stdio.h>

void main(){
    int c;
    bool morethanonce = false;
    while((c=getchar()) != EOF){
         if (c==' ' && morethanonce){
             continue;
         }else(){
             if (c==' '){
                 putchar(c);
                 morethanonce = true;
             }else{
                 putchar(c);
                 morethanonce = false;
             }
         }
    }
}
