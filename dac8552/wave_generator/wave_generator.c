#include "wave_generator.h"

double wavegen_sinusoid_i(unsigned int buffer_point_num, double dc_offset, double amplitude, double initial_phase,
                          unsigned int sample_rate, unsigned int wave_freq, int i)
{
    return (dc_offset + arm_sin_f32(initial_phase + 2.0 * 3.14 * i / (buffer_point_num / ((buffer_point_num * wave_freq) / sample_rate))) * amplitude);
}

int wavegen_sinusoid(unsigned int *dst_buffer, unsigned int buffer_point_num, unsigned int dc_offset, unsigned int amplitude, double initial_phase,
                     unsigned int sample_rate, unsigned int wave_freq)
{
    if (dst_buffer == NULL)
    {
        return -1;
    }

    for (int i = 0; i < buffer_point_num; ++i)
    {
        dst_buffer[i] = wavegen_sinusoid_i(buffer_point_num, dc_offset, amplitude, initial_phase, sample_rate, wave_freq, i);
    }

    return 0;
}

double wavegen_sinusoidbase_square(unsigned int *dst_buffer, unsigned int buffer_point_num, unsigned int dc_offset, unsigned int amplitude, double initial_phase,
                                   unsigned int sample_rate, unsigned int wave_freq, int order)
{
    if (dst_buffer == NULL)
    {
        return -1;
    }

    for (int i = 0; i < buffer_point_num; ++i)
    {
        dst_buffer[i] = 0;
        for (int j = 0; j < order; ++j)
        {
            dst_buffer[i] += wavegen_sinusoid_i(buffer_point_num, dc_offset, 1.0 * amplitude / (2 * j + 1), initial_phase, sample_rate, 1.0 * wave_freq * (2 * j + 1), i);
        }
    }

    return 0;
}