#pragma once

// #include <cstdint>
// #include <cstddef>
#include <type_traits>

namespace utils
{

class SmoothParameter
{
public:

    // Constructor
    SmoothParameter() = delete;
    explicit SmoothParameter(float initValue);

    // Destructor -> default

    // Copy
    SmoothParameter(const SmoothParameter&) = default;
    SmoothParameter& operator=(const SmoothParameter&) = default;

    // Move
    SmoothParameter(SmoothParameter&&) = default;
    SmoothParameter& operator=(SmoothParameter&&) = default;

	//================================================

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

    //================================================

    // Moves the current value to the target value without smoothing
    void prepare();

	//================================================

    // Smooths the current value towards the target value and returns it
    float getSample();

    // Smooths the current value towards the target value across a block of given size and assigns it in place
    void getBlock(float* block, uint32_t numSamples);

    //================================================

private:

    // Updates the current value by smoothing it towards the target
    void update();

    //================================================

    // Minimum value for the smoothingSamples member
    static constexpr uint32_t minSmoothingSamples { 48u };  // 1ms at 48kHz
    // Minimum absolute difference between target and current value. Below it, no smoothing is applied
    static constexpr float minDelta { 1e-9f };

    //================================================

    float currentValue;
    float targetValue;
    uint32_t smoothingSamples;
    float smoothingStep;

    //================================================

    static_assert(std::is_copy_constructible_v<SmoothParameter>, "SmoothParameter must be copyable");
    static_assert(std::is_move_constructible_v<SmoothParameter>, "SmoothParameter must be movable");
    static_assert(std::is_nothrow_move_assignable_v<SmoothParameter>, "Move assignment should not throw");

};

}