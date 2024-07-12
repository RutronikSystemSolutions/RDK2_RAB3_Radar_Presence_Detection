/*
 * custom_alloc.c
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

#include "custom_alloc.h"
#include <stdlib.h>

#include <stdio.h>

static size_t allocated_size = 0;

void* custom_malloc(size_t size)
{
	allocated_size += size;
	printf("custom_malloc: %d\r\n", size);
	printf("custom_malloc total size: %d\r\n", allocated_size);
	return malloc(size);
}

void custom_free(void* ptr)
{
	printf("custom_free\r\n");
	free(ptr);
}


