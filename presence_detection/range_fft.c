/*
 * range_fft.c
 *
 *  Created on: 25 Jun 2024
 *      Author: jorda
 */


#include "range_fft.h"

// TODO remove me
#include <stdio.h>

/**
 * @brief Perform range FFT on the samples contained inside the frame buffer
 *
 * @param [in] frame	Contains the samples (between 0 and 4096) measured by the radar.
 * 						Size of this buffer should be: antenna_count * num_chirps_per_frame * num_samples_per_chirp
 * 						The samples are interleaved
 * 						frame[0] -> antenna 0 sample 0
 * 						frame[1] -> antenna 1 sample 0
 *
 * @param [inout] range	Contains the result of the range FFT computation
 * 						Size of this buffer is antenna_count * num_chirps_per_frame * (num_samples_per_chirp / 2) * sizeof(cfloat32_t)
 * 						range[0] -> antenna 0, chirp 0, range index 0
 * 						range[1] -> antenna 0, chirp 0, range index 1
 *						range[num_samples_per_chirp / 2] -> antenna 0, chirp 1, range index 0
 *
 * @param [in] adc_samples	Buffer used to unpack the samples contained in frame into time buffer used to compute real FFT
 *
 * @param [in] mean_removal	If true, mean will be subtracted from the time buffer
 *
 * @param [in] win		Window to be applied on time buffer before computing FFT
 *
 * @param [in] antenna_count	Number of antennas
 *
 * @param [in] num_samples_per_chirp	Number of ADC samples per chirp
 *
 * @param [in] num_chirps_per_frame		Number of chirps per frame
 *
 * @retval 0 	Success
 * @retval != 0	Error occurred
 */
int range_fft_do(uint16_t* frame,
		cfloat32_t* range,
		float* adc_samples,
		bool mean_removal,
		const float32_t* win,
		uint8_t antenna_count,
		uint16_t num_samples_per_chirp,
		uint16_t num_chirps_per_frame)
{
    if (frame == NULL) return -1;
    if (range == NULL) return -2;

    // Init FFT algorithm
    static arm_rfft_fast_instance_f32 rfft = { 0 };
    if (rfft.fftLenRFFT != num_samples_per_chirp)
    {
        if (arm_rfft_fast_init_f32(&rfft, num_samples_per_chirp) != ARM_MATH_SUCCESS)
        {
            return IFX_SENSOR_DSP_ARGUMENT_ERROR;
        }
    }

    // For each antenna
    for(uint8_t antenna_idx = 0; antenna_idx < antenna_count; ++antenna_idx)
    {
    	// For each chirp
    	for (uint32_t chirp_idx = 0; chirp_idx < num_chirps_per_frame; ++chirp_idx)
		{
    		// The data are interleaved, first need to extract them from the buffer
    		uint16_t start_index = chirp_idx * antenna_count * num_samples_per_chirp;

    		for(uint16_t sample_idx = 0; sample_idx < num_samples_per_chirp; ++sample_idx)
    		{
    			uint16_t index = start_index + sample_idx * antenna_count + antenna_idx;
    			adc_samples[sample_idx] = ((float)frame[index]) / 4096.f; // Copy and directly scale between 0 and 1
    		}

			if (mean_removal)
			{
				ifx_mean_removal_f32(adc_samples, num_samples_per_chirp);
			}

			if (win != NULL)
			{
				arm_mult_f32(adc_samples, win, adc_samples, num_samples_per_chirp);
			}

			arm_rfft_fast_f32(&rfft, adc_samples, (float32_t*)range, 0);
			CIMAG_F32(range[0]) = 0.0f;

			range += (num_samples_per_chirp / 2U);
		}
    }

    return IFX_SENSOR_DSP_STATUS_OK;
}
