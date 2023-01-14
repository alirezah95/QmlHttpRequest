#include <QTest>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class TestQmlHttpRequest : public ::testing::Test
{
public:
    QmlHttpRequest qhr;
};

int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
