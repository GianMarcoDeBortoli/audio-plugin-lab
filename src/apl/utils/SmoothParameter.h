#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace apl
{
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

    // SET METHODS
    // Set the new number of samples needed for smoothing
    void setSmoothingInterval(uint32_t newSmoothingInterval);
    // Set the target value. Allows to skip the smoothing
    void setTarget(float newTargetValue, bool skipSmoothing = false);

    // GET METHODS
    // Returns the number of samples needed for smoothing
    uint32_t getSmoothingInterval() const;
    // Returns the value of the target
    float getTarget() const;
    // Get the current value
    float getCurrentValue() const;

    //================================================
    // STATE METHODS
    // Checks if the parameter needs smoothing or not
    bool isSmoothing() const;
    // Resets the current value to the target value and sets the step size to zero
    void prepare();
    
	//================================================
    // PROCESS METHODS
    // Smooths the current value towards the target value and returns it
    float getNextValue();
    // Smooths the current value towards the target value across a block of given size and assigns it in place
    void getNextValues(float* block, uint32_t numSamples);

    //================================================

private:

    // Updates the current value by smoothing it towards the target
    void update();

    //================================================

    // Default value for the smoothingSamples member
    static constexpr uint32_t defaultSmoothingInterval = 48;
    // Minimum absolute difference between target and current value. Below it, no smoothing is applied
    static constexpr float minDelta { 1e-9f };

    //================================================

    float currentValue;
    float targetValue;
    uint32_t smoothingInterval;
    float stepSize;

};

static_assert(std::is_copy_constructible_v<SmoothParameter>, "SmoothParameter must be copyable");
static_assert(std::is_move_constructible_v<SmoothParameter>, "SmoothParameter must be movable");
static_assert(std::is_nothrow_move_assignable_v<SmoothParameter>, "Move assignment should not throw");

}

}