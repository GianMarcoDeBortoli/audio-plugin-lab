#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cmath>

#include "FirstOrderShelf.h"

namespace apl::operators
{

FirstOrderShelf::FirstOrderShelf(float initHDC, float initHNY, float initCrossFreq) :
    b0 { 480u },
    b1 { 480u },
    a1 { 480u },
    HDC { initHDC },
    HNY { initHNY },
    crossFreq { initCrossFreq },
    prevInput { 0.f },
    prevOutput { 0.f }
{
    assert( initHDC > 0.f && initHDC < 1.f
        && "Magnitude at DC must be in positive with less than unit value" );
    assert( initHNY > 0.f && initHNY < 1.f
        && "Magnitude at NY must be in positive with less than unit value" );
    assert( initCrossFreq > 0.f
        && "Crossover Frequency must be positive" );

    FirstOrderShelf::computeCoefficients();
}

//================================================
// SET METHODS
void FirstOrderShelf::setDCMagnitudeValue(float newHDC)
{
    assert( newHDC > 0.f && newHDC < 1.f
        && "Magnitude at DC must be in positive with less than unit value" );
        
    HDC = newHDC;

    FirstOrderShelf::computeCoefficients();
}

void FirstOrderShelf::setNYMagnitudeValue(float newHNY)
{
    assert( newHNY > 0.f && newHNY < 1.f
        && "Magnitude at NY must be in positive with less than unit value" );

    HNY = newHNY;

    FirstOrderShelf::computeCoefficients();
}

void FirstOrderShelf::setCrossoverFrequency(float newCrossFreq)
{
    assert( newCrossFreq > 0.f
        && "Crossover Frequency must be positive" );

    crossFreq = newCrossFreq;

    FirstOrderShelf::computeCoefficients();
}

// STATE METHODS
void FirstOrderShelf::computeCoefficients() // private
{
    const float t = std::tan(crossFreq);
    const float sqrt_k = std::sqrt( HDC / HNY );
    const float a0_v = t / sqrt_k + 1.0;
    const float b0_v = (t * sqrt_k + 1.0) * HNY / a0_v;
    const float b1_v = (t * sqrt_k - 1.0) * HNY / a0_v;
    const float a1_v = (t / sqrt_k - 1.0) / a0_v;

    b0.setTarget(b0_v, false);
    b1.setTarget(b1_v, false);
    a1.setTarget(a1_v, false);
}

void FirstOrderShelf::clear()
{
    prevInput = 0.f;
    prevOutput = 0.f;
}

void FirstOrderShelf::prepare()
{
    b0.prepare();
    b1.prepare();
    a1.prepare();
}

//================================================
// PROCESS METHODS
float FirstOrderShelf::processSample(float inSample)
{   
    // Smooth coefficient values
    float b0_v = b0.getNextValue();
    float b1_v = b1.getNextValue();
    float a1_v = a1.getNextValue();

    // Process sample
    float outSample = ( b0_v * inSample + b1_v * prevInput ) - ( a1_v * prevOutput );

    // Update state
    prevInput = inSample;
    prevOutput = outSample;

    return outSample;
}

void FirstOrderShelf::processBlock(float* outBlock, const float* inBlock, uint32_t numSamples)
{
    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        outBlock[n] = FirstOrderShelf::processSample(inBlock[n]);
    }
}

}