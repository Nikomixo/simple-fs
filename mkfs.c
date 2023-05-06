#include <unistd.h>

#include "mkfs.h"
#include "block.h"
#include "image.h"

void mkfs(void)
{
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        unsigned char block[BLOCK_SIZE] = {0};
        write(image_fd, block, BLOCK_SIZE);
    }

    for (int i = 0; i < 7; i++)
    {
        alloc();
    }
}