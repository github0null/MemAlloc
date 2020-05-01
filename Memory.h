#ifndef _H_MEMORY
#define _H_MEMORY

#include <stdint.h>
#include <cmsis_compiler.h>

void *malloc(unsigned int size);
void free(void *ptr);
void *calloc(unsigned int nmemb, unsigned int size);
void *realloc(void *ptr, unsigned int size);

#endif