/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "SmoothParameter.h"

//==============================================================================
/**
*/
class SmoothParameterAudioProcessor  : public juce::AudioProcessor,
									                     public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    SmoothParameterAudioProcessor();
    ~SmoothParameterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;

    static constexpr float minParameterValue { 0.0f };
    static constexpr float maxParameterValue { 1000.0f };
    static constexpr float defaultParameterValue { 200.0f };

    static constexpr uint32_t minSmoothingTime { 40u };
    static constexpr uint32_t maxSmoothingTime { 12000u };
    static constexpr uint32_t defaultSmoothingTime { 480u };

private:

	  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	  void parameterChanged(const juce::String& paramID, float newValue) override;

    utils::SmoothParameter parameter;

	  bool isSmoothing = false;
	  bool wasSmoothing = false;

	  uint64_t sampleCounter = 0;
	  uint64_t previousSampleCount = 0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmoothParameterAudioProcessor)
};
