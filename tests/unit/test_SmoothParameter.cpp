#include <gtest/gtest.h>

#include "utils/SmoothParameter.h"

#include <array>
#include <type_traits>
#include <utility>

using apl::utils::SmoothParameter;


// ============================================================
// Construction
// ============================================================

TEST(SmoothParameter, InitializesToGivenValue)
{
    SmoothParameter parameter{0.5f};

    EXPECT_FLOAT_EQ(parameter.getCurrentValue(), 0.5f);
    EXPECT_FLOAT_EQ(parameter.getTarget(), 0.5f);
}

TEST(SmoothParameter, DefaultSmoothingIntervalIsSet)
{
    SmoothParameter parameter{0.0f};

    EXPECT_EQ(parameter.getSmoothingInterval(), 48u);
}

TEST(SmoothParameter, IsNotSmoothingAfterConstruction)
{
    SmoothParameter parameter{0.0f};

    EXPECT_FALSE(parameter.isSmoothing());
}


// ============================================================
// Smoothing interval
// ============================================================

TEST(SmoothParameter, CanSetSmoothingInterval)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(100);

    EXPECT_EQ(parameter.getSmoothingInterval(), 100u);
}

TEST(SmoothParameter, ZeroSmoothingIntervalIsRejected)
{
    SmoothParameter parameter{0.0f};

    EXPECT_THROW(
        parameter.setSmoothingInterval(0),
        std::invalid_argument
    );
}


// ============================================================
// Target
// ============================================================

TEST(SmoothParameter, SettingTargetStartsSmoothing)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(4);
    parameter.setTarget(1.0f);

    EXPECT_FLOAT_EQ(parameter.getTarget(), 1.0f);
    EXPECT_TRUE(parameter.isSmoothing());
    EXPECT_FLOAT_EQ(parameter.getCurrentValue(), 0.0f);
}

TEST(SmoothParameter, SettingCurrentValueAsTargetDoesNotStartSmoothing)
{
    SmoothParameter parameter{1.0f};

    parameter.setTarget(1.0f);

    EXPECT_FALSE(parameter.isSmoothing());
    EXPECT_FLOAT_EQ(parameter.getCurrentValue(), 1.0f);
}

TEST(SmoothParameter, SkipSmoothingJumpsImmediatelyToTarget)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(10);
    parameter.setTarget(1.0f, true);

    EXPECT_FLOAT_EQ(parameter.getCurrentValue(), 1.0f);
    EXPECT_FLOAT_EQ(parameter.getTarget(), 1.0f);
    EXPECT_FALSE(parameter.isSmoothing());
}


// ============================================================
// getNextValue
// ============================================================

TEST(SmoothParameter, GetNextValueMovesTowardsTarget)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(10);
    parameter.setTarget(1.0f);

    const float firstValue = parameter.getNextValue();

    EXPECT_GT(firstValue, 0.0f);
    EXPECT_LT(firstValue, 1.0f);
}

TEST(SmoothParameter, GetNextValueMovesInCorrectDirection)
{
    SmoothParameter parameter{1.0f};

    parameter.setSmoothingInterval(10);
    parameter.setTarget(0.0f);

    const float firstValue = parameter.getNextValue();

    EXPECT_LT(firstValue, 1.0f);
    EXPECT_GT(firstValue, 0.0f);
}

TEST(SmoothParameter, SmoothingEventuallyStops)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(10);
    parameter.setTarget(1.0f);

    for (int i = 0; i < 100; ++i)
        parameter.getNextValue();

    EXPECT_FALSE(parameter.isSmoothing());
}

TEST(SmoothParameter, ReachesHigherTargetAfterSmoothingInterval)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(4);
    parameter.setTarget(1.0f);

    EXPECT_FLOAT_EQ(parameter.getNextValue(), 0.25f);
    EXPECT_FLOAT_EQ(parameter.getNextValue(), 0.50f);
    EXPECT_FLOAT_EQ(parameter.getNextValue(), 0.75f);
    EXPECT_FLOAT_EQ(parameter.getNextValue(), 1.00f);

    EXPECT_FLOAT_EQ(parameter.getCurrentValue(), 1.00f);
    EXPECT_FALSE(parameter.isSmoothing());
}

TEST(SmoothParameter, ReachesLowerTargetAfterSmoothingInterval)
{
    SmoothParameter parameter{1.0f};

    parameter.setSmoothingInterval(4);
    parameter.setTarget(0.0f);

    EXPECT_FLOAT_EQ(parameter.getNextValue(), 0.75f);
    EXPECT_FLOAT_EQ(parameter.getNextValue(), 0.50f);
    EXPECT_FLOAT_EQ(parameter.getNextValue(), 0.25f);
    EXPECT_FLOAT_EQ(parameter.getNextValue(), 0.00f);

    EXPECT_FLOAT_EQ(parameter.getCurrentValue(), 0.00f);
    EXPECT_FALSE(parameter.isSmoothing());
}


// ============================================================
// getNextValues
// ============================================================

TEST(SmoothParameter, GetNextValuesFillsBlock)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(4);
    parameter.setTarget(1.0f);

    std::array<float, 4> block{};

    parameter.getNextValues(block.data(),
                            static_cast<uint32_t>(block.size()));

    for (std::size_t i = 1; i < block.size(); ++i)
        EXPECT_GT(block[i], block[i - 1]);
}

TEST(SmoothParameter, GetNextValuesMatchesGetNextValue)
{
    SmoothParameter blockParameter{0.0f};
    SmoothParameter sampleParameter{0.0f};

    blockParameter.setSmoothingInterval(10);
    sampleParameter.setSmoothingInterval(10);

    blockParameter.setTarget(1.0f);
    sampleParameter.setTarget(1.0f);

    std::array<float, 10> block{};

    blockParameter.getNextValues(
        block.data(),
        static_cast<uint32_t>(block.size())
    );

    for (std::size_t i = 0; i < block.size(); ++i)
    {
        EXPECT_FLOAT_EQ(
            block[i],
            sampleParameter.getNextValue()
        );
    }
}


// ============================================================
// prepare / reset behavior
// ============================================================

TEST(SmoothParameter, PrepareJumpsToTarget)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(10);
    parameter.setTarget(1.0f);

    ASSERT_TRUE(parameter.isSmoothing());

    parameter.prepare();

    EXPECT_FLOAT_EQ(parameter.getCurrentValue(), 1.0f);
    EXPECT_FLOAT_EQ(parameter.getTarget(), 1.0f);
    EXPECT_FALSE(parameter.isSmoothing());
}

TEST(SmoothParameter, PrepareStopsFurtherMovement)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(10);
    parameter.setTarget(1.0f);

    parameter.prepare();

    const float value = parameter.getCurrentValue();

    parameter.getNextValue();

    EXPECT_FLOAT_EQ(parameter.getCurrentValue(), value);
}


// ============================================================
// Stateful behavior
// ============================================================

TEST(SmoothParameter, ChangingTargetMidSmoothingChangesDirection)
{
    SmoothParameter parameter{0.0f};

    parameter.setSmoothingInterval(10);
    parameter.setTarget(1.0f);

    parameter.getNextValue();

    const float beforeChange = parameter.getCurrentValue();

    parameter.setTarget(0.0f);

    const float afterChange = parameter.getNextValue();

    EXPECT_LT(afterChange, beforeChange);
}


// ============================================================
// Copy / move semantics
// ============================================================

TEST(SmoothParameter, IsCopyConstructible)
{
    static_assert(std::is_copy_constructible_v<SmoothParameter>);

    SmoothParameter original{0.5f};
    original.setSmoothingInterval(10);
    original.setTarget(1.0f);

    SmoothParameter copy{original};

    EXPECT_FLOAT_EQ(
        copy.getCurrentValue(),
        original.getCurrentValue()
    );

    EXPECT_FLOAT_EQ(
        copy.getTarget(),
        original.getTarget()
    );

    EXPECT_EQ(
        copy.getSmoothingInterval(),
        original.getSmoothingInterval()
    );
}

TEST(SmoothParameter, CopyAssignmentWorks)
{
    SmoothParameter original{0.5f};
    original.setSmoothingInterval(10);
    original.setTarget(1.0f);

    SmoothParameter copy{0.0f};

    copy = original;

    EXPECT_FLOAT_EQ(
        copy.getCurrentValue(),
        original.getCurrentValue()
    );

    EXPECT_FLOAT_EQ(
        copy.getTarget(),
        original.getTarget()
    );

    EXPECT_EQ(
        copy.getSmoothingInterval(),
        original.getSmoothingInterval()
    );
}

TEST(SmoothParameter, IsMoveConstructible)
{
    static_assert(std::is_move_constructible_v<SmoothParameter>);
    static_assert(std::is_nothrow_move_constructible_v<SmoothParameter>);

    SmoothParameter original{0.5f};

    SmoothParameter moved{std::move(original)};

    EXPECT_FLOAT_EQ(moved.getCurrentValue(), 0.5f);
}

TEST(SmoothParameter, MoveAssignmentWorks)
{
    static_assert(std::is_nothrow_move_assignable_v<SmoothParameter>);

    SmoothParameter original{0.5f};
    SmoothParameter moved{0.0f};

    moved = std::move(original);

    EXPECT_FLOAT_EQ(moved.getCurrentValue(), 0.5f);
}