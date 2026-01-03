/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayLineAudioProcessor::DelayLineAudioProcessor() :
    parameters {*this, nullptr, "PARAMS", createParameterLayout()},
    delayLine {maxDelay, defaultDelay}
{
    parameters.addParameterListener("delayValue", this);
}

DelayLineAudioProcessor::~DelayLineAudioProcessor()
{
}

//==============================================================================
void DelayLineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    delayLine.prepare();
}

void DelayLineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    delayLine.clear();
}

void DelayLineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < 1; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        const int numSamples = buffer.getNumSamples();
        
        for (uint32_t n = 0; n < static_cast<uint32_t>(numSamples); n++)
        {
            delayLine.processSample(&channelData[n], &channelData[n]);
        }
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout DelayLineAudioProcessor::createParameterLayout()
{   
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterInt>(
            juce::ParameterID { "delayValue", 1 },
            "DelayValue",
            minDelay,
            maxDelay,
            defaultDelay
        )
    );

    return { params.begin(), params.end() };
}

void DelayLineAudioProcessor::parameterChanged(const juce::String& paramID, float newValue)
{
    if (paramID == "delayValue")
    {
        const uint32_t delaySamples = static_cast<uint32_t>(std::round(newValue));
        delayLine.setDelay(delaySamples);
    }
}

void DelayLineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayLineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
bool DelayLineAudioProcessor::hasEditor() const {return true;}
juce::AudioProcessorEditor* DelayLineAudioProcessor::createEditor() {return new DelayLineAudioProcessorEditor (*this);}
const juce::String DelayLineAudioProcessor::getName() const {return JucePlugin_Name;}
bool DelayLineAudioProcessor::acceptsMidi() const {return false;}
bool DelayLineAudioProcessor::producesMidi() const {return false;}
bool DelayLineAudioProcessor::isMidiEffect() const{return false;}
double DelayLineAudioProcessor::getTailLengthSeconds() const {return 0.0;}
int DelayLineAudioProcessor::getNumPrograms() {return 1;}
int DelayLineAudioProcessor::getCurrentProgram() {return 0;}
void DelayLineAudioProcessor::setCurrentProgram (int index) {}
const juce::String DelayLineAudioProcessor::getProgramName (int index) {return {};}
void DelayLineAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayLineAudioProcessor();
}
