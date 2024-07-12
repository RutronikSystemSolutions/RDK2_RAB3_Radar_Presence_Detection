/*
 * range_fft.h
 *
 *  Created on: 25 Jun 2024
 *      Author: jorda
 */

#ifndef PRESENCE_DETECTION_RANGE_FFT_H_
#define PRESENCE_DETECTION_RANGE_FFT_H_

#include "ifx_sensor_dsp.h"

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
 * 						Size of this buffer is antenna_count * num_chirps_per_frame * (num_samples_per_chirp / 2)
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
		uint16_t num_chirps_per_frame);

#endif /* PRESENCE_DETECTION_RANGE_FFT_H_ */
