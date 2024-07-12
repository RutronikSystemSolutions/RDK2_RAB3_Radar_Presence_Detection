/*
 * custom_alloc.c
 *
 *  Created on: 25 Jun 2024
 *      Author: jorda
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


