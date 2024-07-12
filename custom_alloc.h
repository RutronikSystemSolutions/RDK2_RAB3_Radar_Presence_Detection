/*
 * custom_alloc.h
 *
 *  Created on: 25 Jun 2024
 *      Author: jorda
 */

#ifndef CUSTOM_ALLOC_H_
#define CUSTOM_ALLOC_H_

#include <stddef.h>

void* custom_malloc(size_t size);

void custom_free(void* ptr);


#endif /* CUSTOM_ALLOC_H_ */
