/*
 * custom_alloc.h
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

#ifndef CUSTOM_ALLOC_H_
#define CUSTOM_ALLOC_H_

#include <stddef.h>

void* custom_malloc(size_t size);

void custom_free(void* ptr);


#endif /* CUSTOM_ALLOC_H_ */
