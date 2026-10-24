#include "PluginProcessor.h"

#include "PluginEditor.h"

FS_SaturationAudioProcessor::FS_SaturationAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
    driveParameter = apvts.getRawParameterValue("drive");
    mixParameter = apvts.getRawParameterValue("mix");
    oversamplingParameter = apvts.getRawParameterValue("oversampling");

    oversampling2x = make_unique<dsp::Oversampling<float>>(2, 1, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
    oversampling4x = make_unique<dsp::Oversampling<float>>(2, 2, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
}

void FS_SaturationAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    currentSampleRate = sampleRate;

    const int choice = (oversamplingParameter != nullptr) ? static_cast<int>(oversamplingParameter->load()) : 0;

    double effectiveSampleRate = sampleRate;
    if (choice == 1)
        effectiveSampleRate *= 2.0;
    else if (choice == 2)
        effectiveSampleRate *= 4.0;

    saturator.prepare(effectiveSampleRate);

    if (oversampling2x != nullptr) {
        oversampling2x->initProcessing(static_cast<size_t>(samplesPerBlock));
        oversampling2x->reset();
    }
    if (oversampling4x != nullptr) {
        oversampling4x->initProcessing(static_cast<size_t>(samplesPerBlock));
        oversampling4x->reset();
    }

    lastOversamplingChoice = choice;

    if (choice == 1 && oversampling2x != nullptr)
        setLatencySamples(oversampling2x->getLatencyInSamples());
    else if (choice == 2 && oversampling4x != nullptr)
        setLatencySamples(oversampling4x->getLatencyInSamples());
    else
        setLatencySamples(0);
}

void FS_SaturationAudioProcessor::releaseResources() {
    if (oversampling2x != nullptr)
        oversampling2x->reset();
    if (oversampling4x != nullptr)
        oversampling4x->reset();
    saturator.reset();
}

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

    const int choice = (oversamplingParameter != nullptr) ? static_cast<int>(oversamplingParameter->load()) : 0;

    if (choice != lastOversamplingChoice) {
        if (oversampling2x != nullptr)
            oversampling2x->reset();
        if (oversampling4x != nullptr)
            oversampling4x->reset();
        saturator.reset();

        double effectiveRate = currentSampleRate;
        if (choice == 1) {
            effectiveRate *= 2.0;
            if (oversampling2x != nullptr)
                setLatencySamples(oversampling2x->getLatencyInSamples());
        } else if (choice == 2) {
            effectiveRate *= 4.0;
            if (oversampling4x != nullptr)
                setLatencySamples(oversampling4x->getLatencyInSamples());
        } else {
            setLatencySamples(0);
        }

        saturator.prepare(effectiveRate);
        lastOversamplingChoice = choice;
    }

    switch (choice) {
    case 1: {
        dsp::AudioBlock<float> block(buffer);
        auto oversampledBlock = oversampling2x->processSamplesUp(block);
        saturator.processBlock(oversampledBlock.getChannelPointer(0), oversampledBlock.getChannelPointer(1),
                               static_cast<int>(oversampledBlock.getNumSamples()));
        oversampling2x->processSamplesDown(block);
        break;
    }
    case 2: {
        dsp::AudioBlock<float> block(buffer);
        auto oversampledBlock = oversampling4x->processSamplesUp(block);
        saturator.processBlock(oversampledBlock.getChannelPointer(0), oversampledBlock.getChannelPointer(1),
                               static_cast<int>(oversampledBlock.getNumSamples()));
        oversampling4x->processSamplesDown(block);
        break;
    }
    default:
        saturator.processBlock(leftChannel, rightChannel, buffer.getNumSamples());
        break;
    }
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

    params.push_back(make_unique<AudioParameterChoice>(ParameterID{"oversampling", 1}, "Oversampling",
                                                       StringArray{"Off", "2x", "4x"}, 0));

    return {params.begin(), params.end()};
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new FS_SaturationAudioProcessor();
}