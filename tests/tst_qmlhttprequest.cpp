#include <QTest>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "qmlhttprequest.hpp"
#include "request.hpp"

class QmlHttpRequestTestable : public qhr::QmlHttpRequest
{ };

class TestQmlHttpRequest : public ::testing::Test
{
public:
    QmlHttpRequestTestable qhr;
};

TEST_F(TestQmlHttpRequest, TestNewRequest)
{
    ASSERT_FALSE(qhr.newRequest() == nullptr);
}

TEST_F(TestQmlHttpRequest, TestDefaultTimeout)
{
    qhr.setDefaultTimeout(3000);
    auto request = qhr.newRequest();
    ASSERT_EQ(request->timeout(), 3000);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
