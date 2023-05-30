#include <unistd.h>
#include <string.h>

#include "mkfs.h"
#include "directory.h"
#include "block.h"
#include "image.h"
#include "inode.h"
#include "pack.h"

void mkfs(void)
{
    // create root directory
    struct inode *root = ialloc();
    int data = alloc();

    root->flags = 2;
    root->size = 2 * ENTRY_SIZE;
    root->block_ptr[0] = data;

    unsigned char block[BLOCK_SIZE];

    // create "." entry
    write_u16(block, root->inode_num);
    strcpy((char *)block + 2, ".");

    // create ".." entry
    write_u16(block + ENTRY_SIZE, root->inode_num);
    strcpy((char *)block + ENTRY_SIZE + 2, "..");

    bwrite(root->inode_num, block);
    iput(root);

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