#include <JuceHeader.h>

namespace param
{
namespace ids
{
    static const juce::String revT60 { "revT60" };
    static const juce::String revBrightness { "revBrightness" };
    static const juce::String revCrossover { "revCrossover" };
    static const juce::String delayFactor { "delayFactor" };
    static const juce::String tvEnable { "tvEnable" };
    static const juce::String oscFreq { "oscFreq" };
    static const juce::String oscDepth { "oscDepth" };
    static const juce::String oscSpread { "oscSpread" };
}

namespace names
{
    static const juce::String revT60 { "revT60" };
    static const juce::String revBrightness { "revBrightness" };
    static const juce::String revCrossover { "revCrossover" };
    static const juce::String delayFactor { "delayFactor" };
    static const juce::String tvEnable { "tvEnable" };
    static const juce::String oscFreq { "oscFreq" };
    static const juce::String oscDepth { "oscDepth" };
    static const juce::String oscSpread { "oscSpread" };
}

namespace ranges
{
    static constexpr float revT60Default { 4.f };
    static constexpr float revT60Min { 0.1f };
    static constexpr float revT60Max { 10.f };
    static constexpr float revT60Inc { 0.1f };
    static constexpr float revT60Skw { 0.5f };

    static constexpr float revBrightnessDefault { 0.5f };
    static constexpr float revBrightnessMin { 0.f };
    static constexpr float revBrightnessMax { 1.f };
    static constexpr float revBrightnessInc { 0.01f };
    static constexpr float revBrightnessSkw { 0.5f };

    static constexpr float revCrossoverDefault { 0.5f };
    static constexpr float revCrossoverMin { 0.f };
    static constexpr float revCrossoverMax { 1.f };
    static constexpr float revCrossoverInc { 0.01f };
    static constexpr float revCrossoverSkw { 0.5f };

    static constexpr float delayFactorDefault { 0.3f };
    static constexpr float delayFactorMin { 1.f };
    static constexpr float delayFactorMax { 5.f };
    static constexpr float delayFactorInc { 0.01f };
    static constexpr float delayFactorSkw { 1.f };

    static constexpr bool tvEnableDefault { true };

    static constexpr float oscFreqDefault { 3.f };
    static constexpr float oscFreqMin { 0.01f };
    static constexpr float oscFreqMax { 10.f };
    static constexpr float oscFreqInc { 0.01f };
    static constexpr float oscFreqSkw { 0.5f };

    static constexpr float oscDepthDefault { 0.5f };
    static constexpr float oscDepthMin { 0.01f };
    static constexpr float oscDepthMax { 1.f };
    static constexpr float oscDepthInc { 0.01f };
    static constexpr float oscDepthSkw { 0.5f };

    static constexpr float oscSpreadDefault { 0.5f };
    static constexpr float oscSpreadMin { 0.01f };
    static constexpr float oscSpreadMax { 1.f };
    static constexpr float oscSpreadInc { 0.01f };
    static constexpr float oscSpreadSkw { 0.5f };
}

namespace Units
{
    static const juce::String Seconds { "s" };
    static const juce::String Hz { "Hz" };
}


class ParameterManager
{

public:

    // Constructor
    ParameterManager() = default;

    // Destructor
    ~ParameterManager() = default;

    // Copy
    ParameterManager(const ParameterManager&) = delete;
    ParameterManager& operator=(const ParameterManager&) = delete;

    // Move
    ParameterManager(ParameterManager&&) noexcept = delete;
    ParameterManager& operator=(ParameterManager&&) noexcept = delete;

    //================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() noexcept;

private:

};

}