#include "PluginProcessor.h"
#include "PluginEditor.h"

FDNPluginAudioProcessorEditor::FDNPluginAudioProcessorEditor(FDNPluginAudioProcessor& p) :
    AudioProcessorEditor(&p),
    audioProcessor(p)
{
}

FDNPluginAudioProcessorEditor::~FDNPluginAudioProcessorEditor()
{
}

//==============================================================================
void FDNPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void FDNPluginAudioProcessorEditor::resized()
{
}