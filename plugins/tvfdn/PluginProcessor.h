#pragma once

#include <JuceHeader.h>
#include <Eigen/Dense>

#include "Ramp.h"
#include "Matrix.h"
#include "FDN.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String Mix { "mix" };

        static const juce::String fdnOrder { "fdnOrder" };

        static const juce::String revT60 { "revT60" };
        static const juce::String revBrightness { "revBrightness" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String Mix { "Mix" };

        static const juce::String fdnOrder { "FDN Order" };

        static const juce::String revT60 { "Size" };
        static const juce::String revBrightness { "Brightness" };
    }

    namespace Ranges
    {
        static constexpr bool EnabledDefault { true };
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };

        static constexpr float MixDefault { 0.5f };
        static constexpr float MixMin { 0.f };
        static constexpr float MixMax { 1.f };
        static constexpr float MixInc { 0.01f };
        static constexpr float MixSkw { 0.5f };

        static const juce::StringArray fdnOrders { "4", "8", "16" };

        static constexpr float T60Default { 4.f };
        static constexpr float T60Min { 0.1f };
        static constexpr float T60Max { 10.f };
        static constexpr float T60Inc { 0.1f };
        static constexpr float T60Skw { 0.5f };

        static constexpr float BrightnessDefault { 0.5f };
        static constexpr float BrightnessMin { 0.f };
        static constexpr float BrightnessMax { 1.f };
        static constexpr float BrightnessInc { 0.01f };
        static constexpr float BrightnessSkw { 0.5f };
    }

    namespace Units
    {
        static const juce::String Seconds { "s" };
        static const juce::String Milliseconds { "ms" };
        static const juce::String Hz { "Hz" };
        static const juce::String dB { "dB" };
    }
}

class FDNPluginAudioProcessor : public juce::AudioProcessor
{
public:
    FDNPluginAudioProcessor();
    ~FDNPluginAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    mrta::ParameterManager& getParameterManager() { return parameterManager; }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    //==============================================================================

    static const unsigned int MaxChannels { 2 };

private:
    double sampleRate { 48000.0 };
    
    mrta::ParameterManager parameterManager;

    DSP::Ramp enableRamp;
    float enabled { 1.f };

    DSP::Ramp mixRamp;
    float mix;

    uint32_t fdnOrder;
    juce::AudioBuffer<float> fdnBuffer;
    DSP::Matrix fdnInputCoupling;
    DSP::FDN fdn;
    DSP::Matrix fdnOutputCoupling;

    float revT60;
    float revBrightness;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FDNPluginAudioProcessor)
};