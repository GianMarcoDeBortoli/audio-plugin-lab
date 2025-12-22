#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class FDNPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FDNPluginAudioProcessorEditor(FDNPluginAudioProcessor&);
    ~FDNPluginAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    FDNPluginAudioProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDNPluginAudioProcessorEditor)
};