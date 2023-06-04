#include "free.h"
#include "block.h"

int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;

    return -1;
}

void set_free(unsigned char *block, int num, int set)
{
    int byte_num = num / 8;
    int bit_num = num % 8;

    block[byte_num] = set ? block[byte_num] | 1UL << bit_num : block[byte_num] & ~(1UL << bit_num);
}

int find_free(unsigned char *block)
{
    int val = -1;

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        val = find_low_clear_bit(block[i]);
        if (val > -1)
        {
            val += i * 8;
            break;
        }
    }

    return val;
}
