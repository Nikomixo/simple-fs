#include "ctest.h"

int adder(int x, int y)
{
    return x + y;
}

int test_adder(void)
{
    CTEST_ASSERT(adder(1, 2) == 3, "testing common case");
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_adder();

    CTEST_RESULTS();

    CTEST_EXIT();
}