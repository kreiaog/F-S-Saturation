#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

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

    double currentSampleRate = 44100.0;

    SaturationEngine saturator;

    atomic<float> *driveParameter = nullptr;
    atomic<float> *mixParameter = nullptr;

    int oversamplingNums = 0;
    unique_ptr<dsp::Oversampling<float>> oversampling2x;
    unique_ptr<dsp::Oversampling<float>> oversampling4x;
    atomic<float> *oversamplingParameter = nullptr;
    int lastOversamplingChoice = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FS_SaturationAudioProcessor)
};