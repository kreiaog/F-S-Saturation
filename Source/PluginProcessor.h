#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

#include "DSP/SaturationEngine.h"

using namespace std;
using namespace juce;

class FS_SaturationAudioProcessor : public juce::AudioProcessor {
public:
    FS_SaturationAudioProcessor();
    ~FS_SaturationAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "F-S Saturation"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String &) override {}

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState &getAPVTS() { return apvts; }

private:
    AudioProcessorValueTreeState apvts;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    SaturationEngine saturator;

    atomic<float> *driveParameter = nullptr;
    atomic<float> *mixParameter = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FS_SaturationAudioProcessor)
};