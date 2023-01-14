#include <QTest>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "qmlhttprequest.hpp"

class TestQmlHttpRequest : public ::testing::Test
{
public:
    qhr::QmlHttpRequest qhr;
};

TEST_F(TestQmlHttpRequest, TestNewRequest)
{
    ASSERT_FALSE(qhr.newRequest() == nullptr);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
