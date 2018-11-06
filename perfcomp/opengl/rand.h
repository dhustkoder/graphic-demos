#ifndef CXX_RAND_H_
#ifdef __cplusplus
extern "C" {
#endif
float randlf(float min, float max);
void randlf_arr(const float* intervals, float* result, int count);
void init_random_engine(void);
#ifdef __cplusplus
}
#endif
#endif
