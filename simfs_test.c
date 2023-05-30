#include <string.h>
#include <stdio.h>

#include "ctest.h"

#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"
#include "directory.h"
#include "ls.h"

void test_block(void)
{
    image_open("block_testfile", 0);

    unsigned char block_buff[BLOCK_SIZE];
    unsigned char input_buff[BLOCK_SIZE] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

    bwrite(5, input_buff);

    CTEST_ASSERT(memcmp(input_buff, bread(5, block_buff), strlen((char *)input_buff)) == 0, "testing writing and reading from a block");

    CTEST_ASSERT(alloc() == 0, "testing alloc");
    CTEST_ASSERT(alloc() == 1, "testing alloc on non-empty free data map");

    image_close();
}

void test_free(void)
{
    image_open("free_testfile", 0);

    unsigned char block[BLOCK_SIZE] = {0};

    CTEST_ASSERT(find_free(block) == 0, "testing find_free on clean block");

    set_free(block, 0, 1);
    CTEST_ASSERT(find_free(block) == 1, "testing set_free on 0, and find_free on used block");

    set_free(block, 0, 0);
    CTEST_ASSERT(find_free(block) == 0, "set_free on i0 when toggled to 1");

    set_free(block, 1, 1);
    set_free(block, 2, 1);
    set_free(block, 3, 1);
    set_free(block, 4, 1);
    set_free(block, 5, 1);
    CTEST_ASSERT(find_free(block) == 0, "test find_free with trailing 1s");

    image_close();
}

void test_inode(void)
{
    image_open("free_testfile", 0);

    struct inode *node4 = ialloc();
    if (node4 == NULL)
    {
        printf("null\n");
    }
    CTEST_ASSERT(node4->inode_num == 0, "testing with clean inode map");

    struct inode *node5 = ialloc();
    CTEST_ASSERT(node5->inode_num == 1, "testing with used inode map");

    struct inode *node1 = find_incore_free();

    node1->inode_num = 2;
    node1->ref_count = 1;
    node1->owner_id = 1;

    CTEST_ASSERT(find_incore(2)->owner_id == 1, "testing find_incore_free and find_incore");

    node1->size = 5;
    node1->link_count = 2;
    node1->permissions = 1;

    write_inode(node1);

    struct inode node2;
    read_inode(&node2, 2);
    CTEST_ASSERT(node2.permissions == 1, "testing inode_read and inode_write");

    CTEST_ASSERT(iget(2)->ref_count == 2, "testing iget for node stored in core");

    struct inode *node3 = iget(3);
    CTEST_ASSERT(find_incore(3)->ref_count > 0, "testing iget for node not in core");

    iput(node3);
    CTEST_ASSERT(find_incore(3) == NULL, "testing iput for ref_count == 1");

    iget(3);
    iget(3);
    iput(node3);
    CTEST_ASSERT(find_incore(3)->ref_count == 1, "testing iput for ref_count == 2");

    image_close();
}

void test_mkfs(void)
{
    image_open("mkfs_testfile", 0);

    mkfs();

    CTEST_ASSERT(alloc() == 7, "testing mkfs allocates 6 blocks");

    struct directory *dir;
    // open root directory
    dir = directory_open(0);

    struct directory_entry dir_ent;
    directory_get(dir, &dir_ent);

    CTEST_ASSERT(strcmp(dir_ent.name, ".") == 0, "testing mkfs root directory '.' filename, along with directory_open and directory_get");
    CTEST_ASSERT(dir_ent.inode_num == 0, "testing mkfs root directory '.' file points to root inode");

    directory_get(dir, &dir_ent);

    CTEST_ASSERT(strcmp(dir_ent.name, "..") == 0, "testing directory_get grabs next directory entry");

    CTEST_ASSERT(directory_get(dir, &dir_ent) == -1, "testing directory_get returns -1 when out of entries");

    directory_close(dir);

    printf("\ncalling ls()\n");
    ls();

    image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_block();
    test_free();
    test_inode();
    test_mkfs();

    CTEST_RESULTS();

    CTEST_EXIT();
}