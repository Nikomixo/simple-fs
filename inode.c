#include "block.h"
#include "free.h"
#include "inode.h"

int ialloc(void)
{
    unsigned char inode_block[BLOCK_SIZE] = {0};

    bread(INODE_MAP_NUM, inode_block);

    int free_inode = find_free(inode_block);
    if (free_inode != -1)
    {
        set_free(inode_block, free_inode, 1);
        bwrite(INODE_MAP_NUM, inode_block);
    }

    return free_inode;
}