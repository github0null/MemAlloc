#include "Memory.h"

#define INVALID_PTR ((void *)0xFFFFFFFF)
#define True 1
#define False 0

#define BLOCK_NUM (MemSize / BLOCK_SIZE)
#define BLOCK_LIST_LEN (BLOCK_NUM / 4)

const uint16_t _BlockNum = BLOCK_NUM;
const uint16_t _BlockListLen = BLOCK_LIST_LEN;
uint16_t _usage;
uint8_t _memChanged = True;

/**
 * Block list 
 * 
 * 位 0 表示块是否空闲, 位 1 表示块的开始
 *
 * [ 11 01 01 11 ][ 01 01 00 00 ]
 * 
 * |----byte 0---||----byte 1---|
 * 
 */
uint8_t _blockInfo[BLOCK_LIST_LEN];

/**
 * Memory buffer
*/
uint8_t _mBuf[MemSize];

//--------------------

#define BLOCK_FLAG_OCCUPY 0x40
#define BLOCK_FLAG_START 0x80

__STATIC_INLINE uint8_t _CheckBlockFlag(uint16_t index, uint8_t flag)
{
    uint16_t _blockIndex = index >> 2U;
    return _blockInfo[_blockIndex] & (flag >> ((index % 4) << 1));
}

__STATIC_INLINE void _SetBlockFlag(uint16_t index, uint8_t flag)
{
    uint16_t _blockIndex = index >> 2U, _bitMask = flag >> ((index % 4) << 1);
    _blockInfo[_blockIndex] |= _bitMask;
}

__STATIC_INLINE void _ClearBlockFlag(uint16_t index, uint8_t flag)
{
    uint16_t _blockIndex = index >> 2U, _bitMask = (~flag) >> ((index % 4) << 1);
    _blockInfo[_blockIndex] &= _bitMask;
}

//----------------------------------------

void *malloc(unsigned int size)
{
    uint16_t assignNum = (uint16_t)((size / BLOCK_SIZE) + ((size % BLOCK_SIZE) == 0U ? 0U : 1U));
    uint16_t blockIndex, blockEndIndex, _border = _BlockNum - assignNum, n;

    for (blockIndex = 0; blockIndex <= _border;)
    {
        blockEndIndex = blockIndex + assignNum;

        for (n = blockIndex; n < blockEndIndex; n++)
        {
            if (_CheckBlockFlag(n, BLOCK_FLAG_OCCUPY))
                break;
        }

        if (n == blockEndIndex) // found free blocks
        {
            _SetBlockFlag(blockIndex, BLOCK_FLAG_OCCUPY | BLOCK_FLAG_START);

            for (n = blockEndIndex - 1; n > blockIndex; n--)
            {
                _SetBlockFlag(n, BLOCK_FLAG_OCCUPY);
            }

            _memChanged = True;

            return (void *)(_mBuf + (uint32_t)blockIndex * BLOCK_SIZE);
        }

        blockIndex = n + 1;
    }

    // error !, out of memory
    ErrorHandler(MEM_ERR_OUT_OF_MEMORY);

    return INVALID_PTR;
}

void free(void *ptr)
{
    uint32_t addr = (uint32_t)ptr - (uint32_t)_mBuf;
    uint16_t blockIndex;

    if (((uint32_t)ptr < (uint32_t)_mBuf) || (addr >= MemSize))
    {
        // error !, invalid ptr
        ErrorHandler(MEM_ERR_INVALID_PTR);
        return;
    }

    if (addr % BLOCK_SIZE == 0)
    {
        blockIndex = addr / BLOCK_SIZE;

        if (!_CheckBlockFlag(blockIndex, BLOCK_FLAG_START))
        {
            // error !, invalid block ptr
            ErrorHandler(MEM_ERR_INVALID_HEAD_PTR);
            return;
        }

        do
        {
            // free block
            _ClearBlockFlag(blockIndex, BLOCK_FLAG_START | BLOCK_FLAG_OCCUPY);

        } while (++blockIndex < _BlockNum && !_CheckBlockFlag(blockIndex, BLOCK_FLAG_START) &&
                 _CheckBlockFlag(blockIndex, BLOCK_FLAG_OCCUPY));

        _memChanged = True;
    }
}

void *calloc(unsigned int nmemb, unsigned int size)
{
    uint32_t _size = nmemb * size, i = 0;
    void *ptr = malloc(_size);

    if (ptr != INVALID_PTR)
    {
        while (i < _size)
            ((uint8_t *)ptr)[i++] = 0x00;
    }

    return ptr;
}

void *realloc(void *ptr, unsigned int size)
{
    void *newPtr = malloc(size);
    if (newPtr == INVALID_PTR)
    {
        free(ptr);
        return INVALID_PTR;
    }

    uint32_t addr = (uint32_t)ptr - (uint32_t)_mBuf;
    uint16_t blockIndex, fillIndex = 0;

    if (((uint32_t)ptr < (uint32_t)_mBuf) || (addr >= MemSize) || (addr % BLOCK_SIZE != 0))
    {
        // error !, invalid ptr
        ErrorHandler(MEM_ERR_INVALID_PTR);
        free(ptr);
        free(newPtr);
        return INVALID_PTR;
    }

    blockIndex = addr / BLOCK_SIZE;

    if (!_CheckBlockFlag(blockIndex, BLOCK_FLAG_START))
    {
        // error !, invalid block ptr
        ErrorHandler(MEM_ERR_INVALID_HEAD_PTR);
        free(ptr);
        free(newPtr);
        return INVALID_PTR;
    }

    do
    {
        // free block
        _ClearBlockFlag(blockIndex, BLOCK_FLAG_START | BLOCK_FLAG_OCCUPY);

        // copy old value to new location
        for (int16_t i = 0; i < BLOCK_SIZE; i++, fillIndex++)
        {
            ((uint8_t *)newPtr)[fillIndex] = _mBuf[(blockIndex * BLOCK_SIZE) + i];
        }

    } while (++blockIndex < _BlockNum && !_CheckBlockFlag(blockIndex, BLOCK_FLAG_START) &&
             _CheckBlockFlag(blockIndex, BLOCK_FLAG_OCCUPY));

    _memChanged = True;

    return newPtr;
}

float MemUsage(void)
{
    uint16_t i, j;

    if (_memChanged)
    {
        _memChanged = False;
        _usage = 0;

        for (i = 0; i < _BlockListLen; i++)
        {
            uint8_t cBlockInfo = _blockInfo[i];
            for (j = 0; j < 4; j++)
            {
                if (cBlockInfo & 0x1) // block in used
                {
                    _usage++;
                }
                cBlockInfo >>= 2;
            }
        }
    }

    return _usage / (float)_BlockNum;
}
