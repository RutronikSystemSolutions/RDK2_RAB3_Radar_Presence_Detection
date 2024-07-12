/*
 * doppler_fft.c
 *
 *  Created on: 9 Jul 2024
 *      Author: jorda
 */

#include "doppler_fft.h"

int32_t doppler_fft_bin_do(cfloat32_t* range,
		cfloat32_t* doppler,
		bool mean_removal,
		const float32_t* win,
		uint16_t bin_index,
		uint16_t antenna_index,
		uint16_t num_chirps_per_frame,
		uint16_t range_fft_len)
{
    if (range == NULL) return -1;
    if (doppler == NULL) return 2;

    static arm_cfft_instance_f32 cfft = { 0 };
    if (cfft.fftLen != num_chirps_per_frame)
    {
        if (arm_cfft_init_f32(&cfft, num_chirps_per_frame) != ARM_MATH_SUCCESS)
        {
            return IFX_SENSOR_DSP_ARGUMENT_ERROR;
        }
    }

    // Construct the source array -> computation of FFT in place
    const uint16_t start_index = antenna_index * num_chirps_per_frame * range_fft_len;
    for (uint16_t chirp_idx = 0; chirp_idx < num_chirps_per_frame; ++chirp_idx)
    {
    	doppler[chirp_idx] = range[start_index + chirp_idx * range_fft_len + bin_index];
    }

    // Mean removal
    if (mean_removal)
	{
		ifx_cmplx_mean_removal_f32(doppler, num_chirps_per_frame);
	}

    // Windowing
    if (win != NULL)
	{
    	arm_cmplx_mult_real_f32((float32_t*)doppler,
    			win,
				(float32_t*)doppler,
				num_chirps_per_frame);
	}

    // Complex FFT
    arm_cfft_f32(&cfft, (float32_t*)doppler, 0, 1);

    // Remark: to be correct, we should shift the buffer, to center the 0 frequency

    return IFX_SENSOR_DSP_STATUS_OK;
}

