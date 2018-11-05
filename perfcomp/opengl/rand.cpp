#include <random>

std::random_device rd;
std::mt19937 mt;

extern "C" double randlf(double min, double max)
{
	std::mt19937 e2(rd());
	std::uniform_real_distribution<double> dist(min, max);
	return dist(e2);
}

extern "C" void randlf_arr(const double* const intervals, double* const result, const int count)
{
	for (int i = 0; i < count; ++i) {
		std::uniform_real_distribution<double> dist(
		    intervals[i * 2],
		    intervals[i * 2 + 1]
		);
		result[i] = dist(mt);
	}
}


extern "C" void init_random_engine(void)
{
	mt = std::mt19937(rd());
}

