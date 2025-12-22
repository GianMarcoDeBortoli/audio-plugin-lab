#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cstdint>

static const std::vector<mrta::ParameterInfo> Parameters
{
    { Param::ID::Enabled,       Param::Name::Enabled,       Param::Ranges::EnabledOff, Param::Ranges::EnabledOn,  Param::Ranges::EnabledDefault },
    { Param::ID::Mix,           Param::Name::Mix,           "",                    Param::Ranges::MixDefault,        Param::Ranges::MixMin,        Param::Ranges::MixMax,        Param::Ranges::MixInc,        Param::Ranges::MixSkw },
    // { Param::ID::fdnOrder,      Param::Name::fdnOrder,      Param::Ranges::fdnOrders,  0 },
    { Param::ID::revT60,        Param::Name::revT60,        Param::Units::Seconds, Param::Ranges::T60Default,        Param::Ranges::T60Min,        Param::Ranges::T60Max,        Param::Ranges::T60Inc,        Param::Ranges::T60Skw },
    { Param::ID::revBrightness, Param::Name::revBrightness, "",                    Param::Ranges::BrightnessDefault, Param::Ranges::BrightnessMin, Param::Ranges::BrightnessMax, Param::Ranges::BrightnessInc, Param::Ranges::BrightnessSkw }
};

FDNPluginAudioProcessor::FDNPluginAudioProcessor() :
    parameterManager(*this, ProjectInfo::projectName, Parameters),
    enableRamp { 480u }, // TODO redo Ramp default constructor
    enabled { Param::Ranges::EnabledDefault ? 1.f : 0.f },
    mixRamp { 480u }, // TODO redo Ramp default constructor
    mix { Param::Ranges::MixDefault },
    fdnOrder { uint32_t { 16u } },
    fdnInputCoupling { static_cast<int>(fdnOrder), getTotalNumInputChannels() },
    fdn { fdnOrder, Param::Ranges::T60Default, Param::Ranges::BrightnessDefault },
    fdnOutputCoupling { getTotalNumOutputChannels(), static_cast<int>(fdnOrder) }
{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f ? 1.f : 0.f;
        enableRamp.setTarget(enabled, force);
    });
    parameterManager.registerParameterCallback(Param::ID::Mix,
    [this](float newValue, bool /*force*/)
    {
        jassert(newValue >= Param::Ranges::MixMin && newValue <= Param::Ranges::MixMax && "Mix must be in range");
        mix = newValue;
        mixRamp.setTarget(mix);
    });
    parameterManager.registerParameterCallback(Param::ID::revT60,
    [this](float newValue, bool /*force*/)
    {
        jassert(newValue >= Param::Ranges::T60Min && newValue <= Param::Ranges::T60Max && "T60 must be in range");
        fdn.setT60(newValue);
    });
    parameterManager.registerParameterCallback(Param::ID::revBrightness,
    [this](float newValue, bool /*force*/)
    {
        jassert(newValue >= Param::Ranges::BrightnessMin && newValue <= Param::Ranges::BrightnessMax && "Brightness must be in range");
        fdn.setBrightness(newValue);
    });
}

FDNPluginAudioProcessor::~FDNPluginAudioProcessor()
{
}

void FDNPluginAudioProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    jassert(newSampleRate > 0.0 && "Sample rate must be greater than zero");
    jassert(samplesPerBlock > 0 && "Samples per block must be greater than zero");

    sampleRate = newSampleRate;

    enableRamp.prepare(newSampleRate, samplesPerBlock);

    mixRamp.prepare(newSampleRate, samplesPerBlock);

    const int numInputChannels  = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    fdnInputCoupling.prepare(fdnOrder, numInputChannels);
    fdnOutputCoupling.prepare(numOutputChannels, fdnOrder);

    fdn.prepare(newSampleRate, samplesPerBlock);

    fdnBuffer.setSize(std::max(numInputChannels, numOutputChannels), samplesPerBlock);

    parameterManager.updateParameters(true);
}

void FDNPluginAudioProcessor::releaseResources()
{
    // This function will be called when playback stops or is about to start again.
    // Here you can use this as an opportunity to free up any spare memory, etc.
    fdnBuffer.clear();
    fdn.clear();
}

void FDNPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const uint32_t numInputChannels  = static_cast<uint32_t>( getTotalNumInputChannels() );
    const uint32_t numOutputChannels = static_cast<uint32_t>( getTotalNumOutputChannels() );
    const uint32_t trackChannels = std::max(numInputChannels, numOutputChannels);
    const uint32_t numSamples { static_cast<uint32_t>( buffer.getNumSamples() ) };

    std::vector<float> fdnInputFrame(numInputChannels);
    std::vector<float> inBetweenFrame(fdnOrder);
    std::vector<float> fdnOutputFrame(numOutputChannels);

    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        for (int ch = 0; ch < static_cast<int>(numInputChannels); ++ch)
            fdnInputFrame[ch] = buffer.getReadPointer(ch)[n];
        // FDN input coupling
        fdnInputCoupling.processSample(inBetweenFrame.data(), fdnInputFrame.data(), fdnOrder, numInputChannels);
        // FDN process
        fdn.process(inBetweenFrame.data(), inBetweenFrame.data(), fdnOrder);
        // FDN output coupling
        fdnOutputCoupling.processSample(fdnOutputFrame.data(), inBetweenFrame.data(), numOutputChannels, fdnOrder);
        for (int ch = 0; ch < static_cast<int>(numOutputChannels); ++ch)
            fdnBuffer.getWritePointer(ch)[n] = fdnOutputFrame[ch];

    }

    enableRamp.multiplyBuffer(fdnBuffer.getArrayOfWritePointers(), fdnBuffer.getArrayOfReadPointers(), trackChannels, numSamples);
    mixRamp.multiplyBuffer(fdnBuffer.getArrayOfWritePointers(), fdnBuffer.getArrayOfReadPointers(), numOutputChannels, numSamples);

    for (int ch = 0; ch < static_cast<int>(numOutputChannels); ++ch)
        buffer.copyFrom(ch, int { 0 }, fdnBuffer, ch, int { 0 }, static_cast<int>( numSamples ));
}

void FDNPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void FDNPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
bool FDNPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* FDNPluginAudioProcessor::createEditor() { return new FDNPluginAudioProcessorEditor(*this); }
const juce::String FDNPluginAudioProcessor::getName() const { return JucePlugin_Name; }
bool FDNPluginAudioProcessor::acceptsMidi() const { return false; }
bool FDNPluginAudioProcessor::producesMidi() const { return false; }
bool FDNPluginAudioProcessor::isMidiEffect() const { return false; }
double FDNPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int FDNPluginAudioProcessor::getNumPrograms() { return 1; }
int FDNPluginAudioProcessor::getCurrentProgram() { return 0; }
void FDNPluginAudioProcessor::setCurrentProgram(int) { }
const juce::String FDNPluginAudioProcessor::getProgramName (int) { return {}; }
void FDNPluginAudioProcessor::changeProgramName (int, const juce::String&) { }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FDNPluginAudioProcessor();
}