#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "juce_audio_processors_headless/juce_audio_processors_headless.h"

#include <cstdint>
#include <stdexcept>

FDNPluginAudioProcessor::FDNPluginAudioProcessor() :
    parameters {*this, nullptr, "Parameters", param::ParameterManager::createParameterLayout()},
    fdnOrder { uint32_t { 64u } },
    inputCoupling { static_cast<int>(fdnOrder), getTotalNumInputChannels() },
    outputCoupling { getTotalNumOutputChannels(), static_cast<int>(fdnOrder) },
    tvMatrix {static_cast<size_t>(fdnOrder),
        param::ranges::oscFreqDefault,
        param::ranges::oscDepthDefault,
        param::ranges::oscSpreadDefault,
        rng},
    feedbackMatrix {static_cast<int>(fdnOrder)},
    revT60 { param::ranges::revT60Default },
    revBrightness { param::ranges::revBrightnessDefault },
    tvEnabled { param::ranges::tvEnableDefault ? true : false }
{
    // Initialize delay lines
    initDelayLengths = initializeDelayLengths(rng);
    std::vector<size_t> initMaxDelayLengths (static_cast<size_t>(fdnOrder));
    for (size_t i = 0; i < static_cast<size_t>(fdnOrder); ++i)
        initMaxDelayLengths[i] = static_cast<size_t>(initDelayLengths[i]);
    delayLines = std::make_unique<apl::operators::MultichannelDelay>(
        fdnOrder,
        initMaxDelayLengths,
        initDelayLengths
    );
    // Initialize absorption filters
    std::vector<std::pair<float, float>> initAbsorptionValues = computeAbsorptionMagValues(
        revT60,
        revBrightness,
        sampleRate
    );
    std::vector<float> absorptionMagDC(static_cast<size_t>(fdnOrder));
    std::vector<float> absorptionMagNY(static_cast<size_t>(fdnOrder));
    for (size_t i = 0; i < static_cast<size_t>(fdnOrder); ++i)
    {
        absorptionMagDC[i] = initAbsorptionValues[i].first;
        absorptionMagNY[i] = initAbsorptionValues[i].second;
    }
    absorptionFilters = std::make_unique<apl::operators::MultichannelAbsorption>(
        fdnOrder,
        absorptionMagDC,
        absorptionMagNY,
        param::ranges::revCrossoverDefault
    );
    // Initialize buffers
    delaysInFrame.resize(fdnOrder, 0.f);
    delaysOutFrame.resize(fdnOrder, 0.f);
    absorptionOutFrame.resize(fdnOrder, 0.f);
    tvMatrixOutFrame.resize(fdnOrder, 0.f);
    feedbackMatrixOutFrame.resize(fdnOrder, 0.f);
    // Add parameter listeners to the juce::AudioProcessorValueTreeState
    parameters.addParameterListener(param::ids::revT60, this);
    parameters.addParameterListener(param::ids::revBrightness, this);
    parameters.addParameterListener(param::ids::revCrossover, this);
    parameters.addParameterListener(param::ids::delayFactor, this);
    parameters.addParameterListener(param::ids::tvEnable, this);
    parameters.addParameterListener(param::ids::oscFreq, this);
    parameters.addParameterListener(param::ids::oscDepth, this);
    parameters.addParameterListener(param::ids::oscSpread, this);
}

FDNPluginAudioProcessor::~FDNPluginAudioProcessor()
{
}

//==============================================================================
void FDNPluginAudioProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;

    const int numInputChannels  = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();

    inputCoupling.prepare(fdnOrder, numInputChannels);
    delayLines->prepare();
    absorptionFilters->prepare();
    tvMatrix.prepare();
    feedbackMatrix.prepare(fdnOrder);
    outputCoupling.prepare(numOutputChannels, fdnOrder);

    inputFrame.resize(numInputChannels, 0.f);
    outputFrame.resize(numOutputChannels, 0.f);
}

void FDNPluginAudioProcessor::releaseResources()
{
    // This function will be called when playback stops or is about to start again.
    // Here you can use this as an opportunity to free up any spare memory, etc.
    std::fill(inputFrame.begin(), inputFrame.end(), 0.f);
    std::fill(delaysInFrame.begin(), delaysInFrame.end(), 0.f);
    std::fill(delaysOutFrame.begin(), delaysOutFrame.end(), 0.f);
    std::fill(absorptionOutFrame.begin(), absorptionOutFrame.end(), 0.f);
    std::fill(tvMatrixOutFrame.begin(), tvMatrixOutFrame.end(), 0.f);
    std::fill(feedbackMatrixOutFrame.begin(), feedbackMatrixOutFrame.end(), 0.f);
    std::fill(outputFrame.begin(), outputFrame.end(), 0.f);
    delayLines->clear();
    absorptionFilters->clear();
}

//==============================================================================
void FDNPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    const uint32_t numInputChannels  = static_cast<uint32_t>( getTotalNumInputChannels() );
    const uint32_t numOutputChannels = static_cast<uint32_t>( getTotalNumOutputChannels() );
    const uint32_t numSamples { static_cast<uint32_t>( buffer.getNumSamples() ) };

    for (size_t n = 0; n < static_cast<size_t>(numSamples); ++n)
    {
        for (int ch = 0; ch < static_cast<int>(numInputChannels); ++ch)
            inputFrame[ch] = buffer.getReadPointer(ch)[n];
        // FDN input coupling
        inputCoupling.processSample(delaysInFrame.data(), inputFrame.data(), fdnOrder, numInputChannels);
        // FDN process
        delayLines->getSample(delaysOutFrame.data(), fdnOrder);
        absorptionFilters->processSample(absorptionOutFrame.data(), delaysOutFrame.data(), fdnOrder);
        if (tvEnabled)
            tvMatrix.processSample(tvMatrixOutFrame.data(), absorptionOutFrame.data());
        else
            tvMatrixOutFrame = absorptionOutFrame;
        feedbackMatrix.processSample(feedbackMatrixOutFrame.data(), tvMatrixOutFrame.data(), fdnOrder, fdnOrder);
        for (int ch = 0; ch < static_cast<int>(fdnOrder); ++ch)
            delaysInFrame[ch] += feedbackMatrixOutFrame[ch];
        delayLines->setSample(delaysInFrame.data(), fdnOrder);
        delayLines->advancePointer(fdnOrder);
        // FDN output coupling
        outputCoupling.processSample(outputFrame.data(), tvMatrixOutFrame.data(), numOutputChannels, fdnOrder);
        for (int ch = 0; ch < static_cast<int>(numOutputChannels); ++ch)
            buffer.getWritePointer(ch)[n] = outputFrame[ch];

    }
}

//==============================================================================
void FDNPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // parameterManager.getStateInformation(destData);
}

void FDNPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
std::vector<float> FDNPluginAudioProcessor::initializeDelayLengths(std::mt19937 &rng) const
{
    std::vector<float> delayLengths (fdnOrder);

    // Define min and max delay length values (replace with your desired values)
    const size_t minDelay = 300; // example minimum
    const size_t maxDelay = 3000; // example maximum

    // Seed random generator (could use std::random_device for more randomness)
    std::uniform_real_distribution<float> dist(minDelay, maxDelay);

    for (size_t i = 0; i < static_cast<size_t>(fdnOrder); ++i)
    {
        delayLengths[i] = dist(rng);
    }
    
    return delayLengths;
}

//==============================================================================
std::vector<std::pair<float, float>> FDNPluginAudioProcessor::computeAbsorptionMagValues(
    float T60DC,
    float brightness,
    double sampleRate
) const
{
    std::vector<std::pair<float, float>> absorptionMagnitudeValues(static_cast<size_t>(fdnOrder));
    std::vector<float> delayLengths = delayLines->getDelayLinesLengths();

    // Calculate the magnitude values for each filter
    for (size_t i = 0; i < static_cast<size_t>(fdnOrder); ++i)
    {
        float magDCdB = delayLengths[i] * ( -60.f / ( T60DC * static_cast<float>(sampleRate) )) ;
        float magDClinear = std::powf(10.f, magDCdB / 20.f);
        float T60Ny = T60DC * brightness;
        float magNYdB = delayLengths[i] * ( -60.f / ( T60Ny * static_cast<float>(sampleRate) )) ;
        float magNYlinear = std::powf(10.f, magNYdB / 20.f);

        std::pair<float, float> values(magDClinear, magNYlinear);
        absorptionMagnitudeValues[i] = values;
    }

    return absorptionMagnitudeValues;
}

void FDNPluginAudioProcessor::setAbsorptionMagValues(std::vector<std::pair<float, float>> newAbsorptionMagValues) // std::vector<T>.reserve() might throw
{
    std::vector<float> absorptionMagDC(static_cast<size_t>(fdnOrder));
    std::vector<float> absorptionMagNY(static_cast<size_t>(fdnOrder));
    for (size_t i = 0; i < static_cast<size_t>(fdnOrder); ++i)
    {
        absorptionMagDC[i] = newAbsorptionMagValues[i].first;
        absorptionMagNY[i] = newAbsorptionMagValues[i].second;
    }
    absorptionFilters->setDCMagnitudeValue(absorptionMagDC);
    absorptionFilters->setNYMagnitudeValue(absorptionMagNY);
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

void FDNPluginAudioProcessor::parameterChanged(const juce::String& paramID, float newValue)
{
    if (paramID == param::ids::revT60)
    {
        revT60 = newValue;
        std::vector<std::pair<float, float>> absorptionMagValues = FDNPluginAudioProcessor::computeAbsorptionMagValues(newValue, revBrightness, sampleRate);
        FDNPluginAudioProcessor::setAbsorptionMagValues(absorptionMagValues);
    }
    else if (paramID == param::ids::revBrightness)
    {
        revBrightness = newValue;
        std::vector<std::pair<float, float>> absorptionMagValues = FDNPluginAudioProcessor::computeAbsorptionMagValues(revT60, newValue, sampleRate);
        FDNPluginAudioProcessor::setAbsorptionMagValues(absorptionMagValues);
    }
    else if (paramID == param::ids::revCrossover)
    {
        absorptionFilters->setCrossFrequency(newValue);
    }
    else if (paramID == param::ids::delayFactor)
    {
        std::vector<float> newDelayLengths(static_cast<size_t>(fdnOrder));
        for (size_t i = 0; i < static_cast<size_t>(fdnOrder); ++i)
        {
            newDelayLengths[i] = initDelayLengths[i] * newValue;
        }
        delayLines->setDelayLinesLengths(newDelayLengths);
    }
    else if (paramID == param::ids::tvEnable)
    {
        tvEnabled = (newValue > 0.5f) ? true : false;
    }
    else if (paramID == param::ids::oscFreq)
    {
        tvMatrix.setCyclesPerSample(newValue);
    }
    else if (paramID == param::ids::oscDepth)
    {
        tvMatrix.setDepth(newValue);
    }
    else if (paramID == param::ids::oscSpread)
    {
        tvMatrix.setSpread(newValue);
    }
    else
    {
        throw std::invalid_argument("The addressed parameter is not listed");
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FDNPluginAudioProcessor();
}