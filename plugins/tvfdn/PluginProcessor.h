#pragma once

#include <JuceHeader.h>
#include <Eigen/Dense>
#include <memory>

#include "operators/Matrix.h"
#include "operators/MultichannelDelay.h"
#include "operators/MultichannelAbsorption.h"
#include "operators/TimeVaryingMatrix.h"

#include "ParameterManager.h"


class FDNPluginAudioProcessor : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener
{
public:
    FDNPluginAudioProcessor();
    ~FDNPluginAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

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
    juce::AudioProcessorValueTreeState parameters;

    //==============================================================================

private:

    std::vector<float> initializeDelayLengths(std::mt19937 &rng) const;

    std::vector<std::pair<float, float>> computeAbsorptionMagValues(
        float T60DC,
        float brightness,
        double sampleRate
    ) const;
    void setAbsorptionMagValues(std::vector<std::pair<float, float>> newAbsorptionMagValues);

    //==============================================================================
    double sampleRate { 48000.0 };

    //==============================================================================
    uint32_t fdnOrder;
    apl::operators::Matrix inputCoupling;
    std::unique_ptr<apl::operators::MultichannelDelay> delayLines;
    std::unique_ptr<apl::operators::MultichannelAbsorption> absorptionFilters;
    apl::operators::Matrix feedbackMatrix;
    apl::operators::TimeVaryingMatrix tvMatrix;
    std::mt19937 rng { (static_cast<unsigned int>(std::time(nullptr))) };
    apl::operators::Matrix outputCoupling;

    //==============================================================================
    std::vector<float> inputFrame;
    std::vector<float> delaysInFrame;
    std::vector<float> delaysOutFrame;
    std::vector<float> absorptionOutFrame;
    std::vector<float> tvMatrixOutFrame;
    std::vector<float> feedbackMatrixOutFrame;
    std::vector<float> outputFrame;

    //==============================================================================
    float revT60;
    float revBrightness;
    bool tvEnabled;
    std::vector<float> initDelayLengths;

    //==============================================================================
    void parameterChanged(const juce::String& paramID, float newValue) override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FDNPluginAudioProcessor)
};