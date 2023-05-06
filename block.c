#include <unistd.h>

#include "block.h"
#include "image.h"
#include "free.h"

unsigned char *bread(int block_num, unsigned char *block)
{
    lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET);
    read(image_fd, block, BLOCK_SIZE);

    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET);

    write(image_fd, block, BLOCK_SIZE);
}

int alloc(void)
{
    unsigned char data_block[BLOCK_SIZE];

    bread(BLOCK_MAP_NUM, data_block);

    int free_data = find_free(data_block);
    if (free_data != -1)
    {
        set_free(data_block, free_data, 1);
        bwrite(BLOCK_MAP_NUM, data_block);
    }

    return free_data;
}