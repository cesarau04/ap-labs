#include <stdio.h>

void main(){
    float fahr, celsius;
    int lower, upper, step;
    lower = 0; upper = 100; step = 20;
    celsius = lower;
    printf("celsius  fahr\n");
    while (celsius <= upper) {
        fahr = (celsius*1.8) + 32 ;
        printf("%10.0f. %3.1f\n", celsius, fahr);
        celsius = celsius + step;
    }
}
