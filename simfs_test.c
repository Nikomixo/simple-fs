#include <string.h>
#include <stdio.h>

#include "ctest.h"

#include "image.h"
#include "block.h"
#include "free.h"

void test_block(void)
{
    image_open("block_test", 0);

    unsigned char block_buff[BLOCK_SIZE];
    unsigned char input_buff[BLOCK_SIZE] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

    bwrite(5, input_buff);

    CTEST_ASSERT(memcmp(input_buff, bread(5, block_buff), strlen((char *)input_buff)) == 0, "testing writing and reading from a block");

    image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_block();

    CTEST_RESULTS();

    CTEST_EXIT();
}