/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SmoothParameterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SmoothParameterAudioProcessorEditor (SmoothParameterAudioProcessor&);
    ~SmoothParameterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SmoothParameterAudioProcessor& audioProcessor;

    juce::Slider parameterValueSlider;
    std::unique_ptr<juce::SliderParameterAttachment> parameterValueAttachment;

    juce::Slider smoothingTimeSlider;
    std::unique_ptr<juce::SliderParameterAttachment> smoothingTimeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmoothParameterAudioProcessorEditor)
};
