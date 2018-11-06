#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

float randlf(float min, float max)
{
	float retval;
	do {
		retval = 1.0f*rand()/RAND_MAX*(max -  min) + min;
	} while (!(retval > min && retval < max));
	return retval;
}

void randlf_arr(const float* const intervals, float* const result, const int count)
{
	for (int i = 0; i < count; ++i)
		result[i] = randlf(intervals[i * 2], intervals[i * 2 + 1]);
}

void init_random_engine(void)
{
	srand(time(NULL));
}
