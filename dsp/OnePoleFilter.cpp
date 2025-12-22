#include "OnePoleFilter.h"
#include <cstddef>
#include <cstdint>

namespace DSP
{

OnePoleFilter::OnePoleFilter(float initMagDC, float initMagNY) : 
    b0Ramp { 480u },
    a1Ramp { 480u },
    feedbackState { 0.f }
{
    // Check if the magnitudes at DC and Nyquist are in the valid range
    jassert(initMagDC >= 0.f && initMagDC <= 1.f && "Magnitude at DC must be in [0, 1]");
    jassert(initMagNY >= 0.f && initMagNY <= 1.f && "Magnitude at Nyquist must be in [0, 1]");
    // Initialize the magnitudes
    currentMagDC = initMagDC;
    currentMagNY = initMagNY;

    // Calculate the coefficient based on the magnitudes at DC and Nyquist
    computeCoefficients();

    // Set ramp targets for the coefficients
    b0Ramp.setTarget(b0, true);
    a1Ramp.setTarget(a1, true);
}

OnePoleFilter::~OnePoleFilter()
{
}

void OnePoleFilter::computeCoefficients()
{
    // Calculate the coefficients based on the updated magnitudes
    float r = currentMagDC / currentMagNY;

    a1 = ( 1 -  r ) / ( 1 + r ); 
    b0 = ( 1 - a1 ) * currentMagNY;

    // Set the targets for the ramps
    b0Ramp.setTarget(b0);
    a1Ramp.setTarget(a1);
}

void OnePoleFilter::setMagValues(float newMagDC, float newMagNY)
{
    jassert(newMagDC >= 0.f && newMagDC <= 1.f && "Magnitude at DC must be in [0, 1]");
    jassert(newMagNY >= 0.f && newMagNY <= 1.f && "Magnitude at Nyquist must be in [0, 1]");
    currentMagDC = newMagDC;
    currentMagNY = newMagNY;

    // Calculate the new coefficients based on the updated magnitudes
    computeCoefficients();
}

void OnePoleFilter::prepare(double newSampleRate, int samplesPerBlock)
{
    b0Ramp.prepare(newSampleRate, samplesPerBlock);
    a1Ramp.prepare(newSampleRate, samplesPerBlock);
}


void OnePoleFilter::clear()
{
    // Clear the feedback state for each channel
    feedbackState = 0.f;
}

void OnePoleFilter::processSample(float* output, const float* input)
{   
    // Get the next ramp values
    float b0Value;
    float a1Value;
    b0Ramp.assignSample(&b0Value);
    a1Ramp.assignSample(&a1Value);

    // Process single-channel sample
    *output = ( b0Value * *input ) - ( a1Value * feedbackState );
    feedbackState = *output;
}

void OnePoleFilter::processBuffer(float* output, const float* input, uint32_t numSamples)
{
    // Get the next ramp values
    std::vector<float> b0Value(numSamples);
    std::vector<float> a1Value(numSamples);
    b0Ramp.assignBuffer(b0Value.data(), numSamples);
    a1Ramp.assignBuffer(a1Value.data(), numSamples);

    // Process single-channel buffer
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        output[n] = ( b0Value[n] * input[n] ) - ( a1Value[n] * feedbackState );
        feedbackState = output[n];
    }
}

}