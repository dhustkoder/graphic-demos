#include <random>

std::random_device rd;

extern "C" double randlf(double min, double max)
{
	std::mt19937 e2(rd());
	std::uniform_real_distribution<double> dist(min, max);
	return dist(e2);
}


