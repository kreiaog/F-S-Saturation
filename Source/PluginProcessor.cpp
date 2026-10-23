#include "PluginProcessor.h"

#include "PluginEditor.h"

FS_SaturationAudioProcessor::FS_SaturationAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
    driveParameter = apvts.getRawParameterValue("drive");
    mixParameter = apvts.getRawParameterValue("mix");
}

void FS_SaturationAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    saturator.prepare(sampleRate);
}

void FS_SaturationAudioProcessor::releaseResources() {}

void FS_SaturationAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (totalNumInputChannels == 0 || buffer.getNumSamples() == 0)
        return;

    if (driveParameter != nullptr)
        saturator.setDrive(driveParameter->load());

    if (mixParameter != nullptr)
        saturator.setMix(mixParameter->load() * 0.01f);

    auto *leftChannel = buffer.getWritePointer(0);
    auto *rightChannel = (totalNumInputChannels > 1) ? buffer.getWritePointer(1) : leftChannel;

    saturator.processBlock(leftChannel, rightChannel, buffer.getNumSamples());
}

juce::AudioProcessorEditor *FS_SaturationAudioProcessor::createEditor() {
    return new FS_SaturationAudioProcessorEditor(*this);
}

void FS_SaturationAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
    auto state = apvts.copyState();
    unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FS_SaturationAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
    unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(ValueTree::fromXml(*xmlState));
}

AudioProcessorValueTreeState::ParameterLayout FS_SaturationAudioProcessor::createParameterLayout() {
    vector<unique_ptr<RangedAudioParameter>> params;

    params.push_back(make_unique<AudioParameterFloat>(ParameterID{"drive", 1}, "Drive",
                                                      NormalisableRange<float>(0.0f, 18.0f, 0.1f), 0.0f,
                                                      AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(make_unique<AudioParameterFloat>(ParameterID{"mix", 1}, "Mix",
                                                      NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
                                                      AudioParameterFloatAttributes().withLabel("%")));

    return {params.begin(), params.end()};
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new FS_SaturationAudioProcessor();
}