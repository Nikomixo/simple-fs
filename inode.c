#include <stdio.h>

#include "block.h"
#include "free.h"
#include "inode.h"
#include "pack.h"

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *ialloc(void)
{
    unsigned char inode_block[BLOCK_SIZE] = {0};

    bread(INODE_MAP_NUM, inode_block);

    int free_inode = find_free(inode_block);
    if (free_inode == -1)
        return NULL;

    set_free(inode_block, free_inode, 1);
    bwrite(INODE_MAP_NUM, inode_block);

    struct inode *node = iget(free_inode);
    if (node == NULL)
        return NULL;

    node->size = 0;
    node->owner_id = 0;
    node->permissions = 0;
    node->flags = 0;

    for (int i = 0; i < INODE_PTR_COUNT; i++)
    {
        node->block_ptr[i] = 0;
    }

    write_inode(node);

    return node;
}

struct inode *find_incore_free(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        if (incore[i].ref_count == 0)
        {
            struct inode *node = &incore[i];
            return node;
        }
    }
    return NULL;
}

struct inode *find_incore(unsigned int inode_num)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        if (incore[i].ref_count != 0 && incore[i].inode_num == inode_num)
        {
            struct inode *node = &incore[i];
            return node;
        }
    }
    return NULL;
}

void read_inode(struct inode *in, int inode_num)
{
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = inode_num % INODES_PER_BLOCK;

    unsigned char block_buff[BLOCK_SIZE];
    bread(block_num, block_buff);

    int block_offset_bytes = block_offset * INODE_SIZE;

    in->size = read_u32(block_buff + block_offset_bytes);
    in->owner_id = read_u16(block_buff + block_offset_bytes + 4);
    in->permissions = read_u8(block_buff + block_offset_bytes + 6);
    in->flags = read_u8(block_buff + block_offset_bytes + 7);
    in->link_count = read_u8(block_buff + block_offset_bytes + 8);

    for (int i = 0; i < INODE_PTR_COUNT; i++)
    {
        in->block_ptr[i] = read_u16(block_buff + block_offset_bytes + 9 + (i * 2));
    }
}

void write_inode(struct inode *in)
{
    int block_num = in->inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = in->inode_num % INODES_PER_BLOCK;

    unsigned char block_buff[BLOCK_SIZE];
    bread(block_num, block_buff);

    int block_offset_bytes = block_offset * INODE_SIZE;

    write_u32(block_buff + block_offset_bytes, in->size);
    write_u16(block_buff + block_offset_bytes + 4, in->owner_id);
    write_u8(block_buff + block_offset_bytes + 6, in->permissions);
    write_u8(block_buff + block_offset_bytes + 7, in->flags);
    write_u8(block_buff + block_offset_bytes + 8, in->link_count);

    for (int i = 0; i < INODE_PTR_COUNT; i++)
    {
        write_u16(block_buff + block_offset_bytes + 9 + (i * 2), in->block_ptr[i]);
    }

    bwrite(block_num, block_buff);
}

struct inode *iget(int inode_num)
{
    struct inode *return_node;

    return_node = find_incore(inode_num);
    if (return_node != NULL)
    {
        return_node->ref_count += 1;
        return return_node;
    }

    return_node = find_incore_free();
    if (return_node == NULL)
    {
        return NULL;
    }

    read_inode(return_node, inode_num);
    return_node->ref_count = 1;
    return_node->inode_num = inode_num;

    return return_node;
}

void iput(struct inode *in)
{
    if (in->ref_count > 0)
    {
        in->ref_count -= 1;
        if (in->ref_count == 0)
            write_inode(in);
    }
}