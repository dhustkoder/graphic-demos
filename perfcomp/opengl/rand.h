#ifndef CXX_RAND_H_
extern "C" double randlf(double min, double max);
extern "C" void randlf_arr(const double* intervals, double* result, int count);
extern "C" void init_random_engine(void);
#endif
