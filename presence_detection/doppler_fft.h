/*
 * doppler_fft.h
 *
 *  Created on: 9 Jul 2024
 *      Author: jorda
 *
 * Rutronik Elektronische Bauelemente GmbH Disclaimer: The evaluation board
 * including the software is for testing purposes only and,
 * because it has limited functions and limited resilience, is not suitable
 * for permanent use under real conditions. If the evaluation board is
 * nevertheless used under real conditions, this is done at one’s responsibility;
 * any liability of Rutronik is insofar excluded
 */

#ifndef PRESENCE_DETECTION_DOPPLER_FFT_H_
#define PRESENCE_DETECTION_DOPPLER_FFT_H_

#include "ifx_sensor_dsp.h"

/**
 * @brief Compute the Doppler FFT for the given bin (bin_index)
 *
 * @param [in] range	Array containing the range FFT.
 * 						Size of this buffer is antenna_count * num_chirps_per_frame * (num_samples_per_chirp / 2) * sizeof(cfloat32_t)
 * 						range[0] -> antenna 0, chirp 0, range index 0
 * 						range[1] -> antenna 0, chirp 0, range index 1
 *						range[num_samples_per_chirp / 2] -> antenna 0, chirp 1, range index 0
 *
 * @param [out] doppler	Array containing the doppler FFT for the given bin
 * 						The array must be allocated by the caller function
 * 						Size of this buffer is num_chirps_per_frame * sizeof(cfloat32_t)
 *
 * @param [in] mean_removal	Perform mean removal or not before computing FFT
 * @param [in] win	Window to be applied to the signal before computing FFT
 * @param [in] bin_index	Index of the bin for which the doppler FFT has to be computed [0] to [(num_samples_per_chirp / 2) - 1]
 * @param [in] antenna_index	Index of the antenna for which the doppler FFT has to be computed
 * @param [in] num_chirps_per_frame	Number of chirps per frame
 * @param [in] range_fft_len	Length of the computed range FFT (per chirp - might vary depending on zero padding)
 *
 * @retval 0 On success
 */
int32_t doppler_fft_bin_do(cfloat32_t* range,
		cfloat32_t* doppler,
		bool mean_removal,
		const float32_t* win,
		uint16_t bin_index,
		uint16_t antenna_index,
		uint16_t num_chirps_per_frame,
		uint16_t range_fft_len);

#endif /* PRESENCE_DETECTION_DOPPLER_FFT_H_ */
