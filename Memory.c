#include "Memory.h"

#define INVALID_PTR ((void *)0xFFFFFFFF)

#define BLOCK_NUM (MemSize / BLOCK_SIZE)
#define BLOCK_LIST_LEN (BLOCK_NUM / 4)

const uint16_t _BlockNum = BLOCK_NUM;
const uint16_t _BlockListLen = BLOCK_LIST_LEN;

/** bit 0 位表示是否空闲, bit 1 表示块的开始
*
*    [ 11 01 01 11 ][ 01 01 00 00 ]
*
* bit  10
*
*    |----byte-0---||----byte-1---|
*/
uint8_t _blockInfo[BLOCK_LIST_LEN];
int8_t _mBuf[MemSize];

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

        if (n >= blockEndIndex)
        {
            _SetBlockFlag(blockIndex, BLOCK_FLAG_OCCUPY | BLOCK_FLAG_START);

            for (n = blockEndIndex - 1; n > blockIndex; n--)
            {
                _SetBlockFlag(n, BLOCK_FLAG_OCCUPY);
            }
            return (void *)(_mBuf + (uint32_t)blockIndex * BLOCK_SIZE);
        }
        else
        {
            blockIndex = n + 1;
        }
    }

    // error !, out of memory
    HandleError();

    return INVALID_PTR;
}

void free(void *ptr)
{
    uint32_t addr = (uint32_t)ptr - (uint32_t)_mBuf;
    uint16_t blockIndex;

    if (((uint32_t)ptr < (uint32_t)_mBuf) || (addr >= MemSize))
        return;

    if (addr % BLOCK_SIZE == 0)
    {
        blockIndex = addr / BLOCK_SIZE;

        if (!_CheckBlockFlag(blockIndex, BLOCK_FLAG_START))
        {
            return;
        }

        do
        {
            _ClearBlockFlag(blockIndex, BLOCK_FLAG_START | BLOCK_FLAG_OCCUPY);
        } while (++blockIndex < _BlockNum && !_CheckBlockFlag(blockIndex, BLOCK_FLAG_START));
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
    free(ptr);
    return malloc(size);
}

float MemUsage(void)
{
    uint16_t i, j;
    uint16_t usage = 0;

    for (i = 0; i < _BlockListLen; i++)
    {
        uint8_t cBlockInfo = _blockInfo[i];
        for (j = 0; j < 4; j++)
        {
            if (cBlockInfo & 0x1) // block in used
            {
                usage++;
            }
            cBlockInfo >>= 2;
        }
    }

    return usage / (float)_BlockNum;
}
