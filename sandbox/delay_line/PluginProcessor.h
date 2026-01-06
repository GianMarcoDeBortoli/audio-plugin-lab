/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <DelayLine.h>

//==============================================================================
/**
*/
class DelayLineAudioProcessor : public juce::AudioProcessor,
 								                public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    DelayLineAudioProcessor();
    ~DelayLineAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

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
	  juce::AudioProcessorValueTreeState parameters;

	  static constexpr int minDelay { 0u };
	  static constexpr int maxDelay { 24000u };
	  static constexpr int defaultDelay { 200u };

private:

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	  void parameterChanged(const juce::String& paramID, float newValue) override;

	  primitives::DelayLine delayLine;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayLineAudioProcessor)
};
