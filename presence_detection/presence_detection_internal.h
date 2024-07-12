/*
 * presence_detection_internal.h
 *
 *  Created on: 9 Jul 2024
 *      Author: jorda
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
