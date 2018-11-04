#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

/* generate a random doubleing point number from min to max */
int randnum(int min, int max)
{
   return min + rand() % (max+1 - min);
}


int main(void)
{
	int min, max, div;
	srand(time(NULL));
	scanf("%d %d %d", &min, &max, &div);
	for (int i = 0; i < 10; ++i) {
		double v = randnum(min, max);
		printf("%lf : : %lf\n", v, v / div);
	}
}
