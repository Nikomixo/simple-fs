#include <stdlib.h>
#include <string.h>

#include "directory.h"
#include "dirbasename.h"
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
    int offset_in_block = dir->offset % BLOCK_SIZE;

    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);

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

int directory_make(char *path)
{
    char dirpath[MAX_STRING_LENGTH];
    get_dirname(path, dirpath);

    // Temporary - if path doesn't begin with root, return -1.
    if (strcmp(dirpath, "/") != 0)
        return -1;

    char dirname[MAX_STRING_LENGTH];
    get_basename(path, dirname);

    struct inode *parent_inode = namei(dirpath);
    struct inode *new_dir_inode = ialloc();

    int data = alloc();
    new_dir_inode->flags = 2;
    new_dir_inode->size = 2 * ENTRY_SIZE;
    new_dir_inode->block_ptr[0] = data;

    unsigned char block[BLOCK_SIZE];

    // create "." entry
    write_u16(block, new_dir_inode->inode_num);
    strcpy((char *)block + 2, ".");

    // create ".." entry
    write_u16(block + ENTRY_SIZE, parent_inode->inode_num);
    strcpy((char *)block + ENTRY_SIZE + 2, "..");

    bwrite(new_dir_inode->inode_num, block);

    int parent_block_num = parent_inode->size / BLOCK_SIZE;
    int parent_block_index = parent_inode->size % BLOCK_SIZE;

    unsigned char parent_block[BLOCK_SIZE];
    bread(parent_inode->block_ptr[parent_block_num], parent_block);

    write_u16(parent_block + parent_block_index, new_dir_inode->inode_num);
    strcpy((char *)parent_block + parent_block_index + 2, dirname);

    bwrite(parent_inode->block_ptr[parent_block_num], parent_block);
    parent_inode->size += ENTRY_SIZE;

    iput(parent_inode);
    iput(new_dir_inode);

    return 0;
}

// temporarily only returning root
struct inode *namei(char *path)
{
    if (strcmp(path, "/") == 0)
    {
        struct inode *root_inode = iget(ROOT_INODE_NUM);
        return root_inode;
    }
    else
    {
        // TODO implement later
        return NULL;
    }
}