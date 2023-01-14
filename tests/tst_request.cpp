#include <QTest>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "request.hpp"

class TestRequest : public ::testing::Test
{
public:
    void SetUp() override
    {

    }

    void TearDown() override
    {

    }

    qhr::Request request;
};

TEST_F(TestRequest, TestDefaultConstructedRequest)
{
    ASSERT_FALSE(request.isOpen());
}


int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
