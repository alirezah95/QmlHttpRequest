#include <QTest>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "request.hpp"

TEST(TestRequest, TestDefaultConstructedRequest)
{
    qhr::Request request;
    ASSERT_FALSE(request.isOpen());
}


int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
