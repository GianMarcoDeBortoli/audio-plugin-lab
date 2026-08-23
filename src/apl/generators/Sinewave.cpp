#pragma once

#include <cassert>

#include "Sinewave.h"

namespace primitives
{

Sinewave::Sinewave(float initFrequency) :
    frequency { initFrequency }
{
    amplitude.setSmoothingTime( uint32_t { 200u } );
}

//================================================

void Sinewave::setFrequency(float newFrequency)
{
    assert(newFrequency > 0.0f);
    frequency = newFrequency;
}

//================================================

void Sinewave::prepare()
{

}

//================================================

void Sinewave::fadeOut()
{

}

void Sinewave::fadeIn()
{

}

void Sinewave::processBlock(float* outBlock, float* inBlock, uint32_t numSamples)
{
    for (uint32_t n = 0; n < numSamples; n++)
    {
        Sinewave::processSample(&outBlock[n], &inBlock[n]);
    }
}

}