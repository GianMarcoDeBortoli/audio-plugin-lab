#pragma once

#include <cassert>

#include "Oscillator.h"

namespace primitives
{

Oscillator::Oscillator(float initFrequency) :
    frequency { initFrequency }
{
    amplitude.setSmoothingTime( uint32_t { 200u } );
}

//================================================

void Oscillator::setFrequency(float newFrequency)
{
    assert(newFrequency > 0.0f);
    frequency = newFrequency;
}

//================================================

void Oscillator::prepare()
{

}

//================================================

void Oscillator::fadeOut()
{

}

void Oscillator::fadeIn()
{

}

void Oscillator::processBlock(float* outBlock, float* inBlock, uint32_t numSamples)
{
    for (uint32_t n = 0; n < numSamples; n++)
    {
        Oscillator::processSample(&outBlock[n], &inBlock[n]);
    }
}

}