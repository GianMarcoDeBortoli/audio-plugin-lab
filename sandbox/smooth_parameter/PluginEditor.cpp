/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SmoothParameterAudioProcessorEditor::SmoothParameterAudioProcessorEditor (SmoothParameterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);

    parameterValueSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    parameterValueSlider.setRange (0.0, 127.0, 1.0);
    parameterValueSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    parameterValueSlider.setPopupDisplayEnabled (true, false, this);
    parameterValueSlider.setTextValueSuffix (" Value");
    parameterValueSlider.setValue (1.0);
    addAndMakeVisible(&parameterValueSlider);
    parameterValueAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *audioProcessor.parameters.getParameter("parameterValue"), parameterValueSlider
    );

    smoothingTimeSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    smoothingTimeSlider.setRange (0.0, 127.0, 1.0);
    smoothingTimeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    smoothingTimeSlider.setPopupDisplayEnabled (true, false, this);
    smoothingTimeSlider.setTextValueSuffix (" Time");
    smoothingTimeSlider.setValue (1.0);
    addAndMakeVisible(&smoothingTimeSlider);
    smoothingTimeAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *audioProcessor.parameters.getParameter("smoothingTime"), smoothingTimeSlider
    );
}

SmoothParameterAudioProcessorEditor::~SmoothParameterAudioProcessorEditor()
{
}

//==============================================================================
void SmoothParameterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Value Slider", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void SmoothParameterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    parameterValueSlider.setBounds(40,30,20,getHeight() -60);
    smoothingTimeSlider.setBounds(70,60,20,getHeight() -60);
}
