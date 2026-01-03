/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <iostream>

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SmoothParameterAudioProcessor::SmoothParameterAudioProcessor() :
    parameters(*this, nullptr, "PARAMS", createParameterLayout())
{
    parameter.setSmoothingTime(defaultSmoothingTime);
    parameter.setTarget(defaultParameterValue);
    parameters.addParameterListener("parameterValue", this);
    parameters.addParameterListener("smoothingTime", this);
}

SmoothParameterAudioProcessor::~SmoothParameterAudioProcessor()
{
}

//==============================================================================
const juce::String SmoothParameterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SmoothParameterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SmoothParameterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SmoothParameterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SmoothParameterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SmoothParameterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SmoothParameterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SmoothParameterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SmoothParameterAudioProcessor::getProgramName (int index)
{
    return {};
}

void SmoothParameterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SmoothParameterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    parameter.prepare();
}

void SmoothParameterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void SmoothParameterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    for (int channel = 0; channel < 1; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...

        const uint32_t numSamples = buffer.getNumSamples();
        for (uint32_t n = 0; n < numSamples; ++n)
        {
            bool isSmoothing = parameter.needsSmoothing();
            if (isSmoothing && !wasSmoothing)
            {
                std::cout << "Smoothing started at sample "
                        << sampleCounter
                        << " with value "
                        << parameter.getCurrentValue()
                        << std::endl;
                previousSampleCount = sampleCounter;
            }

            float currentvalue = parameter.getSample();

            sampleCounter++;

            if (!isSmoothing && wasSmoothing)
            {
                std::cout << "Smoothing stopped at sample "
                        << sampleCounter
                        << " with value "
                        << parameter.getCurrentValue()
                        << std::endl;
                std::cout << "It took "
                        << sampleCounter - previousSampleCount
                        << std::endl;
            }

            wasSmoothing = isSmoothing;
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SmoothParameterAudioProcessor::createParameterLayout()
{
    using namespace juce;

    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { "parameterValue", 1 },
        "ParameterValue",
        minParameterValue,
        maxParameterValue,
        defaultParameterValue
    ));
    params.push_back(std::make_unique<AudioParameterInt>(
        ParameterID {"smoothingTime", 2 },
        "SmoothingTime",
        minSmoothingTime,
        maxSmoothingTime,
        defaultSmoothingTime
    ));

    return { params.begin(), params.end() };
}

void SmoothParameterAudioProcessor::parameterChanged(const juce::String& paramID, float newValue)
{   
    if (paramID == "parameterValue")
    {
        std::cout << "Target stored in APVTS: " << *parameters.getRawParameterValue("parameterValue") << std::endl;
        std::cout << "Target before callback: " << parameter.getTarget() << std::endl;
        parameter.setTarget(newValue, false);
        std::cout << "Target after callback: " << parameter.getTarget() << std::endl;
    }
    else if (paramID == "smoothingTime")
    {
        std::cout << "Smoothing time stored in APVTS: " << *parameters.getRawParameterValue("smoothingTime") << std::endl;
        std::cout << "Smoothing time before callback: " << parameter.getSmoothingTime() << std::endl;
        parameter.setSmoothingTime(newValue);
        std::cout << "Smoothing time after callback: " << parameter.getSmoothingTime() << std::endl;
    }
}

//==============================================================================
bool SmoothParameterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SmoothParameterAudioProcessor::createEditor()
{
    return new SmoothParameterAudioProcessorEditor (*this);
}

//==============================================================================
void SmoothParameterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SmoothParameterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SmoothParameterAudioProcessor();
}
