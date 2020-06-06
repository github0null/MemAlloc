#ifndef _H_MEMORY
#define _H_MEMORY

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined(__CC_ARM) ||        \
    defined(__GNUC__) ||        \
    defined(__ARMCC_VERSION) || \
    defined(__GNUC__) ||        \
    defined(__ICCARM__) ||      \
    defined(__CSMC__) ||        \
    defined(__TI_ARM__)
#include <cmsis_compiler.h>
#else
#define __STATIC_INLINE
#endif

#define MEM_SIZE_512B 0x200U
#define MEM_SIZE_1K 0x400U
#define MEM_SIZE_2K 0x800U
#define MEM_SIZE_4K 0x1000U
#define MEM_SIZE_8K 0x2000U
#define MEM_SIZE_16K 0x4000U
#define MEM_SIZE_32K 0x8000U
#define MEM_SIZE_64K 0x10000U
#define MEM_SIZE_128K 0x20000U

/**
 * Heap Size
*/
#if defined(HEAP_SIZE_512B)
#define MemSize MEM_SIZE_512B
#define BLOCK_SIZE 8U
#elif defined(HEAP_SIZE_1K)
#define MemSize MEM_SIZE_1K
#define BLOCK_SIZE 8U
#elif defined(HEAP_SIZE_2K)
#define MemSize MEM_SIZE_2K
#elif defined(HEAP_SIZE_4K)
#define MemSize MEM_SIZE_4K
#elif defined(HEAP_SIZE_8K)
#define MemSize MEM_SIZE_8K
#elif defined(HEAP_SIZE_16K)
#define MemSize MEM_SIZE_16K
#elif defined(HEAP_SIZE_32K)
#define MemSize MEM_SIZE_32K
#elif defined(HEAP_SIZE_64K)
#define MemSize MEM_SIZE_64K
#define BLOCK_SIZE 32U
#elif defined(HEAP_SIZE_128K)
#define MemSize MEM_SIZE_128K
#define BLOCK_SIZE 32U
#else
#define MemSize MEM_SIZE_16K
#endif

/**
 * Block Size
*/
#ifndef BLOCK_SIZE
#define BLOCK_SIZE 16U
#endif

    /**
 * Error Handler
*/

#define MEM_ERR_OUT_OF_MEMORY 1
#define MEM_ERR_INVALID_PTR 2
#define MEM_ERR_INVALID_HEAD_PTR 3

#ifdef NOT_HANDLE_ERROR
#define ErrorHandler(errCode)
#else
#define ErrorHandler(errCode) __breakpoint(0)
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
