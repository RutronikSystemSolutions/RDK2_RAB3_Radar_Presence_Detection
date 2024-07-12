/*
 * presence_detection.h
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

#ifndef PRESENCE_DETECTION_PRESENCE_DETECTION_H_
#define PRESENCE_DETECTION_PRESENCE_DETECTION_H_

#include <stddef.h>
#include <stdint.h>

typedef void* (*malloc_func_t)(size_t size);
typedef void (*free_func_t)(void* ptr);

/**
 * @brief Listener function enabling to get notified when an event occured
 */
typedef void (*presence_detection_listener_func_t)(float magnitude, uint16_t bin, float angle);

typedef struct
{
	float threshold;		/**< Threshold used to detect a presence */

	/**< User can select the range to be controlled
	 * If bin_start == bin_end -> complete range */
	uint16_t bin_start;
	uint16_t bin_end;
} presence_detection_param_t;

typedef struct
{
	uint8_t antenna_count;
	uint16_t chirps_per_frame;
	uint16_t samples_per_chirp;
	uint32_t sampling_rate;
	uint64_t start_freq;
	uint64_t end_freq;
} radar_configuration_t;

void presence_detection_set_malloc_free(malloc_func_t malloc, free_func_t free);

void presence_detection_set_listener(presence_detection_listener_func_t listener);

int presence_detection_init(radar_configuration_t radar_configuration, presence_detection_param_t params);

/**
 * @brief Feed the algorithm with data
 *
 * @param [in] frame_samples	Raw data coming from the BGT60TR13C sensor
 * 								The data are "interleaved":
 * 								frame_samples[0] -> sample 0 of antenna 0
 * 								frame_samples[1] -> sample 0 of antenna 1
 */
void presence_detection_feed(uint16_t * frame_samples);

float presence_detection_bin_to_meters(uint16_t bin);

#endif /* PRESENCE_DETECTION_PRESENCE_DETECTION_H_ */
