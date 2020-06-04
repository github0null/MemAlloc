
#ifndef _H_MEMORY
#define _H_MEMORY

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <cmsis_compiler.h>

#define MEM_SIZE_256B 0x100U
#define MEM_SIZE_512B 0x200U
#define MEM_SIZE_1K 0x400U
#define MEM_SIZE_2K 0x800U
#define MEM_SIZE_4K 0x1000U
#define MEM_SIZE_8K 0x2000U
#define MEM_SIZE_16K 0x4000U
#define MEM_SIZE_32K 0x8000U
#define MEM_SIZE_64K 0x10000U

/**
 * Heap Size
*/
#define MemSize MEM_SIZE_32K

/**
 * Block Size
*/
#define BLOCK_SIZE 16U

/**
 * Error Handler
*/
#ifdef NOT_HANDLE_ERROR
#define HandleError()
#else
#define HandleError() __breakpoint(0)
#endif

void *malloc(unsigned int size);
void free(void *ptr);
void *calloc(unsigned int nmemb, unsigned int size);
void *realloc(void *ptr, unsigned int size);
float MemUsage(void);

#ifdef __cplusplus
}
#endif

#endif
