//
// Created by egordm on 5-2-18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class BaseTests : public ::testing::Test {

};

TEST_F(BaseTests, BaseTests_HelloWorld_Test) {
    std::cout << "Hello, World!" << std::endl;
    ASSERT_EQ(1, 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}