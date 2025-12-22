#pragma once

#include <cstdint>

#include <JuceHeader.h>
#include <Eigen/Dense>


#include "Matrix.h"
#include "MultichannelDelay.h"
#include "MultichannelAbsorption.h"

namespace DSP
{

class FDN
{
public:
    FDN(
        uint32_t initOrder,
        float initT60DC,
        float initBrightness
    );
    ~FDN();

    // No default ctors
    FDN() = delete;

    // No copy semantics
    FDN(const FDN&) = delete;
    const FDN& operator=(const FDN&) = delete;

    // No move semantics
    FDN(FDN&&) = delete;
    const FDN& operator=(FDN&&) = delete;

    // =============================================

    // Constants
    static constexpr uint32_t possibleOrders[] = { 2u, 4u, 8u, 16u, 32u, 64u };

    // =============================================

    // Check if the order is valid
    static uint32_t checkOrder(uint32_t order);

    // Delay Lines
    // Compute the delay line lengths
    std::vector<size_t> computeDelayLengths();
    // Compute the maximum delay lengths for the delay lines
    std::vector<size_t> computeMaxDelayLinesLengths();
    // Set the delay lines lengths
    // void setDelayLinesLengths(const std::vector<size_t>& newDelayLinesLengths);

    // Absorption Filters
    // Compute the absorption filters' magnitude values
    std::vector<std::pair<float, float>> computeAbsorptionMagValues(
        float T60DC,
        float brightness,
        double sampleRate
    );
    // Set the reverberation time at DC and brightness
    void setT60(float newT60DC);
    void setBrightness(float newBrightness);

    // =============================================

    // Prepare state
    void prepare(double newSampleRate, int samplesPerBlock);

    // Clear contents
    void clear();

    // Process audio
    void process(float* output, const float* input, uint32_t numChannels);
    
    // =============================================

private:
    double sampleRate { 48000.0 };

    uint32_t order;

    std::vector<size_t> delayLengths;
    std::vector<size_t> maxDelayLengths;
    std::unique_ptr<DSP::MultichannelDelay> delayLines;
    // TODO: store max time variation for delay lines

    DSP::Matrix feedbackMatrix;
    std::vector<float> feedbackState;

    float T60DC;
    float brightness;
    std::vector<std::pair<float, float>> absorptionMagnitudeValues;
    std::unique_ptr<DSP::MultichannelAbsorption> absorptionFilters;
};

}