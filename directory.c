#include <stdlib.h>
#include <string.h>

#include "directory.h"
#include "inode.h"
#include "block.h"
#include "pack.h"

struct directory *directory_open(int inode_num)
{
    struct inode *directory_node = iget(inode_num);
    if (directory_node == NULL)
    {
        return NULL;
    }

    struct directory *dir = malloc(sizeof(struct directory));
    dir->inode = directory_node;
    dir->offset = 0;

    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    if (dir->offset >= dir->inode->size)
    {
        return -1;
    }

    int data_block_index = dir->offset / BLOCK_SIZE;
    int data_block_num = dir->inode->block_ptr[data_block_index];

    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);

    int offset_in_block = dir->offset % BLOCK_SIZE;

    ent->inode_num = read_u16(block + offset_in_block);
    strcpy(ent->name, (char *)block + offset_in_block + 2);

    dir->offset += ENTRY_SIZE;
    return 0;
}

void directory_close(struct directory *d)
{
    iput(d->inode);
    free(d);
}