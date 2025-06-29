#include <gtest/gtest.h>
#ifdef _WIN64
int main(int argc, char** argv)
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
#endif