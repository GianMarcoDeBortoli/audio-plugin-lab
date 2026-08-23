#include <gtest/gtest.h>

#include "operators/DelayLine.h"


TEST(DelayLineTest, InitialDelayIsCorrect)
{
    apl::operators::DelayLine delay(1000, 100.f);

    EXPECT_FLOAT_EQ(
        delay.getDelayLength(),
        100.0f
    );
}