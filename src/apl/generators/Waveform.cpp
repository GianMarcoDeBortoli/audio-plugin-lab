#pragma once

#include <cassert>

#include "Waveform.h"

namespace generator
{

Waveform::Waveform(float initFrequency) :
    frequency { initFrequency }
{
    amplitude.setSmoothingInterval( uint32_t { 200u } );
}

//================================================

void Waveform::setFrequency(float newFrequency)
{
    assert(newFrequency > 0.0f);
    frequency = newFrequency;
}

//================================================

void Waveform::prepare()
{

}

//================================================

void Waveform::fadeOut()
{

}

void Waveform::fadeIn()
{

}

void Waveform::processBlock(float* outBlock, float* inBlock, uint32_t numSamples)
{
    for (uint32_t n = 0; n < numSamples; n++)
    {
        Waveform::processSample(&outBlock[n], &inBlock[n]);
    }
}

}