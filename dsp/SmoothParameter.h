#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace utils
{

class SmoothParameter
{
public:
    // Constructors
    SmoothParameter();

    // Destructor
    ~SmoothParameter() = default;

    // No copy semantics
    SmoothParameter(const SmoothParameter&) = delete;
    const SmoothParameter& operator=(const SmoothParameter&) = delete;

    // No move semantics
    SmoothParameter(SmoothParameter&&) noexcept = default;
    SmoothParameter& operator=(SmoothParameter&&) noexcept = default;

	//==============================================================================

    // Get the current value
    float getCurrentValue();

    // Set new smoothing time
    void setSmoothingTime(uint32_t newTimeInSamples);

    // Returns the smoothing time
    uint32_t getSmoothingTime();

    // Set the target value. Allows to skip the smoothing
    void setTarget(float newTargetValue, bool skipSmoothing = false);

    // Returns the value of the target
    float getTarget();

    // Checks if the parameter needs smoothing or not
    bool needsSmoothing();

	//==============================================================================

    void prepare();

	//==============================================================================

    // Smooths the current value towards the target value and returns it
    float getSample();

    // Smooths the current value towards the target value across a block of given size and assigns it in place
    void getBlock(float* block, uint32_t numSamples);

	//==============================================================================

    // Minimum SmoothParameter time in seconds ( 1ms)
    static constexpr uint32_t minTimeInSamples { 48u };  // 1ms at 48kHz
    // Minimum absolute difference between target and current value
    static constexpr float minDelta { 1e-9f };

private:

    void update();

    float targetValue;
    uint32_t smoothingSamples;
    float smoothingStep;
    float currentValue;

    // static_assert(std::is_copy_constructible_v<SmoothParameter>);
    // static_assert(std::is_move_constructible_v<SmoothParameter>);
    static_assert(std::is_nothrow_move_assignable_v<SmoothParameter>);
};

}