#include <QTest>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(TestRequest, TestDefaultConstructedRequest)
{
    Request request;
}


int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
