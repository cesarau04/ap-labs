#include <stdio.h>

float fahrtocels(float fahr);

void main(){
	float fahr, celsius;
	int lower, upper, step;
	lower = 0; upper = 300; step = 20;
	fahr = lower;
	printf("fahr celsius\n");
	while (fahr <= upper) {
		celsius = fahrtocels(fahr);
		printf("%3.0f %6.1f\n", fahr, celsius);
		fahr = fahr + step;
	}
}

float fahrtocels(float fahr){
	return (5.0/9.0) * (fahr-32.0);
}
