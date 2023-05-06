#include <string.h>
#include <stdio.h>

#include "ctest.h"

#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

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

    CTEST_ASSERT(ialloc() == 0, "testing with clean inode map");
    CTEST_ASSERT(ialloc() == 1, "testing with used inode map");

    image_close();
}

void test_mkfs(void)
{
    image_open("mkfs_testfile", 0);

    mkfs();

    CTEST_ASSERT(alloc() == 7, "testing mkfs allocates 6 blocks");

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