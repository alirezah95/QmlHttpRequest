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

TEST_F(TestRequest, TestOpenMethod)
{
    request.open("GET", QUrl("https://fake.com"));
    ASSERT_TRUE(request.isOpen());
}

TEST_F(TestRequest, TestSetRequestHeader)
{
    request.open("GET", QUrl("https://fake.com"));
    request.setRequestHeader("Content-type", "application-json");
    ASSERT_STREQ(request.requestHeader("Content-type").constData(), "application-json");
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
