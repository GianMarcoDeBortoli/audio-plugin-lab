#include "ParameterManager.h"

namespace param
{

juce::AudioProcessorValueTreeState::ParameterLayout ParameterManager::createParameterLayout() noexcept
{   
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
            juce::ParameterID { param::ids::revT60, 1 },
            param::names::revT60,
            param::ranges::revT60Min,
            param::ranges::revT60Max,
            param::ranges::revT60Default
        )
    );
    params.push_back(std::make_unique<AudioParameterFloat>(
            juce::ParameterID { param::ids::revBrightness, 2  },
            param::names::revBrightness,
            param::ranges::revBrightnessMin,
            param::ranges::revBrightnessMax,
            param::ranges::revBrightnessDefault
        )
    );
    params.push_back(std::make_unique<AudioParameterFloat>(
            juce::ParameterID { param::ids::revCrossover, 3 },
            param::names::revCrossover,
            param::ranges::revCrossoverMin,
            param::ranges::revCrossoverMax,
            param::ranges::revCrossoverDefault
        )
    );
    params.push_back(std::make_unique<AudioParameterFloat>(
            juce::ParameterID { param::ids::delayFactor, 4 },
            param::names::delayFactor,
            param::ranges::delayFactorMin,
            param::ranges::delayFactorMax,
            param::ranges::delayFactorDefault
        )
    );
    params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID { param::ids::tvEnable, 5 },
            param::names::tvEnable,
            param::ranges::tvEnableDefault
        )
    );
    params.push_back(std::make_unique<AudioParameterFloat>(
            juce::ParameterID { param::ids::oscFreq, 6 },
            param::names::oscFreq,
            param::ranges::oscFreqMin,
            param::ranges::oscFreqMax,
            param::ranges::oscFreqDefault
        )
    );
    params.push_back(std::make_unique<AudioParameterFloat>(
            juce::ParameterID { param::ids::oscDepth, 7 },
            param::names::oscDepth,
            param::ranges::oscDepthMin,
            param::ranges::oscDepthMax,
            param::ranges::oscDepthDefault
        )
    );
    params.push_back(std::make_unique<AudioParameterFloat>(
            juce::ParameterID { param::ids::oscSpread, 8 },
            param::names::oscSpread,
            param::ranges::oscSpreadMin,
            param::ranges::oscSpreadMax,
            param::ranges::oscSpreadDefault
        )
    );

    return { params.begin(), params.end() };
}

}