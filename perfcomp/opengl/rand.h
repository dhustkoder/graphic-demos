#ifndef CXX_RAND_H_
#ifdef __cplusplus
extern "C" {
#endif
double randlf(double min, double max);
void randlf_arr(const double* intervals, double* result, int count);
void init_random_engine(void);
#ifdef __cplusplus
}
#endif
#endif
