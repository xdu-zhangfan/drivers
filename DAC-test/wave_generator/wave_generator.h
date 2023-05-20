#ifndef WAVE_GENERATOR
#define WAVE_GENERATOR

#include "math.h"

#include "arm_math.h"

double wavegen_sinusoid_i(unsigned int buffer_point_num, double dc_offset, double amplitude, double initial_phase,
                          unsigned int sample_rate, unsigned int wave_freq, int i);
int wavegen_sinusoid(unsigned int *dst_buffer, unsigned int buffer_point_num, unsigned int dc_offset, unsigned int amplitude, double initial_phase,
                     unsigned int sample_rate, unsigned int wave_freq);
double wavegen_sinusoidbase_square(unsigned int *dst_buffer, unsigned int buffer_point_num, unsigned int dc_offset, unsigned int amplitude, double initial_phase,
                                   unsigned int sample_rate, unsigned int wave_freq, int order);

#endif