/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayLineAudioProcessorEditor::DelayLineAudioProcessorEditor (DelayLineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    delayValueSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    delayValueSlider.setRange (0.0, 127.0, 1.0);
    delayValueSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    delayValueSlider.setPopupDisplayEnabled (true, false, this);
    delayValueSlider.setTextValueSuffix (" Value");
    delayValueSlider.setValue (1.0);
    addAndMakeVisible(&delayValueSlider);

    delayValueAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *audioProcessor.parameters.getParameter("delayValue"), delayValueSlider
    );
}

DelayLineAudioProcessorEditor::~DelayLineAudioProcessorEditor()
{
}

//==============================================================================
void DelayLineAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Delay Slider", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void DelayLineAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    delayValueSlider.setBounds(40,30,20,getHeight() -60);
}
