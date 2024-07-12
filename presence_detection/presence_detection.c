/*
 * presence_detection.c
 *
 *  Created on: 25 Jun 2024
 *      Author: jorda
 *
 * Rutronik Elektronische Bauelemente GmbH Disclaimer: The evaluation board
 * including the software is for testing purposes only and,
 * because it has limited functions and limited resilience, is not suitable
 * for permanent use under real conditions. If the evaluation board is
 * nevertheless used under real conditions, this is done at one’s responsibility;
 * any liability of Rutronik is insofar excluded
 */

#include "presence_detection.h"
#include "presence_detection_internal.h"
#include "range_fft.h"
#include "doppler_fft.h"

/**
 * @def DEBUG_AMPLITUDE
 * @brief If defined, plot the maximum amplitude and index
 */
#undef DEBUG_AMPLITUDE

/**
 * @def DEBUG_PHASE_CONTENT
 * @brief If defined, plot the phase of the maximum bin
 */
#undef DEBUG_PHASE_CONTENT

#if defined(DEBUG_AMPLITUDE) || defined(DEBUG_PHASE_CONTENT)
#include <stdio.h>
#endif

/**
 * @var internal_malloc
 * Function enabling to allocate memory
 */
static malloc_func_t internal_malloc = NULL;
static free_func_t internal_free = NULL;

/**
 * @var internal_listener
 * Function to be called when an event is detected
 */
static presence_detection_listener_func_t internal_listener = NULL;

static const uint8_t REQUIRED_ANTENNA_COUNT = 2;

/**
 * @var adc_samples
 * Store the converted ADC samples.
 * This buffer is used as source for the range FFT (since the raw frame_samples contains interleaved samples).
 */
static float* adc_samples = NULL;

/**
 * @var window
 * Window used to be applied on the time signal before computing real FFT
 */
static float* window = NULL;

/**
 * @var range
 * Store the output of the range computation
 * Allocated once at start
 * Size is antenna count * chirps per frame * (samples per chirp / 2) * sizeof(cfloat)
 * Why samples per chirp / 2 and not (samples per chirp) / 2 + 1 -> because of the implementation of the FFT
 */
static cfloat32_t* range = NULL;

/**
 * @var doppler_out
 * Store the result of the doppler FFT for one bin
 * doppler_out is the output of the FFT of complex values
 */
static cfloat32_t* doppler_out = NULL;

/**
 * @var internal_params
 * Internal parameters used for the presence detection algorithm
 */
static presence_detection_internal_param_t internal_params;


void presence_detection_set_malloc_free(malloc_func_t malloc, free_func_t free)
{
	internal_malloc = malloc;
	internal_free = free;
}

void presence_detection_set_listener(presence_detection_listener_func_t listener)
{
	internal_listener = listener;
}

int presence_detection_init(radar_configuration_t radar_configuration, presence_detection_param_t params)
{
	if (radar_configuration.antenna_count != REQUIRED_ANTENNA_COUNT) return -1;
	if (internal_free == NULL) return -2;
	if (internal_malloc == NULL) return -3;

	// Save
	internal_params.antenna_count = radar_configuration.antenna_count;
	internal_params.chirps_per_frame = radar_configuration.chirps_per_frame;
	internal_params.samples_per_chirp = radar_configuration.samples_per_chirp;
	internal_params.sampling_rate = radar_configuration.sampling_rate;
	internal_params.start_freq = radar_configuration.start_freq;
	internal_params.end_freq = radar_configuration.end_freq;

	internal_params.threshold = params.threshold;

	// Compute bin_start and bin_end
	if (params.bin_start == params.bin_end)
	{
		// Total range
		const uint16_t fft_len = internal_params.samples_per_chirp / 2;
		internal_params.bin_start = 0;
		internal_params.bin_end = fft_len;
	}
	else if (params.bin_start > params.bin_end)
	{
		return -4;
	}
	else
	{
		internal_params.bin_start = params.bin_start;
		internal_params.bin_end = params.bin_end;
	}

	// Allocate
	adc_samples = (float*) internal_malloc(radar_configuration.samples_per_chirp * sizeof(float));
	if (adc_samples == NULL) return -5;

	range = (cfloat32_t*) internal_malloc(radar_configuration.antenna_count * radar_configuration.chirps_per_frame * (radar_configuration.samples_per_chirp / 2) * sizeof(cfloat32_t));
	if (range == NULL) return -6;

	doppler_out = (cfloat32_t*) internal_malloc(radar_configuration.chirps_per_frame * sizeof(cfloat32_t));
	if (range == NULL) return -7;

	// Generate window
	window = (float*) internal_malloc(radar_configuration.samples_per_chirp * sizeof(float));
	if (window == NULL) return -8;
	ifx_window_blackmanharris_f32(window, radar_configuration.samples_per_chirp);

	return 0;
}

static float get_magnitude(cfloat32_t complex_value)
{
	float32_t* value = (float32_t*)&complex_value;
	float32_t real = value[0];
	float32_t imag = value[1];
	float32_t magnitude = 0;

	arm_sqrt_f32((real * real) + (imag * imag), &magnitude);

	return magnitude;
}

static float get_max_magnitude(cfloat32_t* array, uint16_t len)
{
	float max = 0;
	for(uint16_t i = 0; i < len; ++i)
	{
		float mag = get_magnitude(array[i]);
		if (mag > max) max = mag;
	}
	return max;
}

/**
 * @brief Compute the angle using phase difference
 * This function uses the "range" array which contains FFT output per chirp
 */
static float get_phase_difference_for_bin(uint16_t bin)
{
	// range[0] -> antenna 0, chirp 0, range index 0
	// range[1] -> antenna 0, chirp 0, range index 1
	// range[num_samples_per_chirp / 2] -> antenna 0, chirp 1, range index 0
	// second antenna comes later on

	const uint16_t fft_len = internal_params.samples_per_chirp / 2;
	const uint16_t second_antenna_start_index = internal_params.chirps_per_frame * fft_len;

	float angle_sum = 0;
	for(uint16_t chirp_idx = 0; chirp_idx < internal_params.chirps_per_frame; ++chirp_idx)
	{
		uint16_t antenna0_index = chirp_idx * fft_len + bin;
		uint16_t antenna1_index = second_antenna_start_index + antenna0_index;

		float32_t* value0 = (float32_t*)&(range[antenna0_index]);
		float32_t* value1 = (float32_t*)&(range[antenna1_index]);

		float32_t real0 = value0[0];
		float32_t imag0 = value0[1];
		float phase0 = atan2f(imag0, real0);

		float32_t real1 = value1[0];
		float32_t imag1 = value1[1];
		float phase1 = atan2f(imag1, real1);

		float angle_diff = phase0 - phase1;
		if (angle_diff > M_PI)
		{
			angle_diff -= (2 * M_PI);
		}
		else if (angle_diff < (-M_PI))
		{
			angle_diff += (2 * M_PI);
		}

#ifdef DEBUG_PHASE_CONTENT
		printf("%1.1f\r\n", angle_diff);
#endif

		angle_sum += angle_diff;
	}
	return (angle_sum / (float) internal_params.chirps_per_frame);
}

void presence_detection_feed(uint16_t * frame_samples)
{
	const uint16_t fft_len = internal_params.samples_per_chirp / 2;

	// Compute range FFT of the frame. For each chirp compute a FFT -> output inside "range"
	range_fft_do(frame_samples,
			range,
			adc_samples,
			true,				// remove mean
			window,				// window (Blackman Harris)
			REQUIRED_ANTENNA_COUNT,
			internal_params.samples_per_chirp,
			internal_params.chirps_per_frame);

	// Compute Doppler FFT for each bin (only for antenna 0 to save time)
	// Bin index from [0] to [(samples per chirp / 2) - 1]
	// and extract maximum
	float maximum_doppler = 0;
	uint16_t max_bin_idx = 0;
	for(uint16_t bin_idx = internal_params.bin_start; bin_idx < internal_params.bin_end; ++bin_idx)
	{
		doppler_fft_bin_do(range,
				doppler_out,		// Doppler FFT output (size is chirps_per_frame)
				true,				// Remove mean (0 m/s speed)
				NULL,				// Window
				bin_idx,			// Bin index
				0, 					// Antenna index
				internal_params.chirps_per_frame,
				fft_len);

		// Get maximum amplitude
		float max_magnitude = get_max_magnitude(doppler_out, internal_params.chirps_per_frame);
		if (max_magnitude > maximum_doppler)
		{
			maximum_doppler = max_magnitude;
			max_bin_idx = bin_idx;
		}
	}

#ifdef DEBUG_AMPLITUDE
	printf("%1.1f at index %2d\r\n", maximum_doppler, max_bin_idx);
#endif

	if (maximum_doppler > internal_params.threshold)
	{
		// Above threshold, compute the angle for the max magnitude bin
		float angle = get_phase_difference_for_bin(max_bin_idx);

		// Get angle of arrival
		// Use approximation
		// Teta = (Lambda * Delta Phase) / (2 * Pi * d)
		// For BGT60TR13C, we have d = Lambda / 2
		// Teta = Delta Phase / Pi
		// To be accurate we should use asin()

		angle = angle / M_PI;

		if (internal_listener != NULL)
		{
			internal_listener(maximum_doppler, max_bin_idx, angle);
		}
	}
}

float presence_detection_bin_to_meters(uint16_t bin)
{
	const float bandwidth = (float) internal_params.end_freq - (float) internal_params.start_freq;
	const float fftlen = internal_params.samples_per_chirp / 2;
	const float fractionfs = (float) bin / ((fftlen - 1) * 2);
	const float freq = fractionfs * (float) internal_params.sampling_rate;
	const float slope = bandwidth / ((float)internal_params.samples_per_chirp * (1.f / (float)internal_params.sampling_rate));
	return (299792458.f * freq) / (2.f * slope);
}
