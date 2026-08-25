#include <cstddef>
#include <cstdint>
#include <cassert>

#include "operators/OnePoleFilter.h"

namespace apl::operators
{

OnePoleFilter::OnePoleFilter(float initHDC, float initHNY) :
    b0 { 480u },
    a1 { 480u },
    HDC { initHDC },
    HNY { initHNY },
    prevOutput { 0.f }
{
    assert( initHDC > 0.f && initHDC < 1.f
        && "Magnitude at DC must be in positive with less than unit value" );
    assert( initHNY > 0.f && initHNY < 1.f
        && "Magnitude at NY must be in positive with less than unit value" );

    OnePoleFilter::computeCoefficients();
}

//================================================
// SET METHODS
void OnePoleFilter::setDCMagnitudeValue(float newHDC)
{
    assert( newHDC > 0.f && newHDC < 1.f
        && "Magnitude at DC must be in positive with less than unit value" );
        
    HDC = newHDC;

    OnePoleFilter::computeCoefficients();
}

void OnePoleFilter::setNYMagnitudeValue(float newHNY)
{
    assert( newHNY > 0.f && newHNY < 1.f
        && "Magnitude at NY must be in positive with less than unit value" );

    HNY = newHNY;

    OnePoleFilter::computeCoefficients();
}

// STATE METHODS
void OnePoleFilter::computeCoefficients() // private
{
    const float r = HDC / HNY;
    const float a1_v = ( 1 -  r ) / ( 1 + r );
    const float b0_v = ( 1 - a1_v ) * HNY;

    b0.setTarget(b0_v, false);
    a1.setTarget(a1_v, false);
}

void OnePoleFilter::clear()
{
    prevOutput = 0.f;
}

void OnePoleFilter::prepare()
{
    b0.prepare();
    a1.prepare();
}

//================================================
// PROCESS METHODS
float OnePoleFilter::processSample(float inSample)
{   
    // Smooth coefficient values
    float b0_v = b0.getNextValue();
    float a1_v = a1.getNextValue();

    // Process sample
    float outSample = ( b0_v * inSample ) - ( a1_v * prevOutput );

    // Update state
    prevOutput = outSample;

    return outSample;
}

void OnePoleFilter::processBlock(float* outBlock, const float* inBlock, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        outBlock[n] = OnePoleFilter::processSample(inBlock[n]);
    }
}

}