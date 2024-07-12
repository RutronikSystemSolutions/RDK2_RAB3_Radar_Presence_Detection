/*
 * presence_detection_internal.h
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

#ifndef PRESENCE_DETECTION_PRESENCE_DETECTION_INTERNAL_H_
#define PRESENCE_DETECTION_PRESENCE_DETECTION_INTERNAL_H_

#include <stdint.h>

typedef struct
{
	uint8_t antenna_count;
	uint16_t chirps_per_frame;
	uint16_t samples_per_chirp;
	uint32_t sampling_rate;
	uint64_t start_freq;
	uint64_t end_freq;

	uint16_t bin_start;
	uint16_t bin_end;

	float threshold;
} presence_detection_internal_param_t;

#endif /* PRESENCE_DETECTION_PRESENCE_DETECTION_INTERNAL_H_ */
