#pragma once

#include "GUI/FS_LookAndFeel.h"
#include "PluginProcessor.h"

using namespace juce;

using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;

class FS_SaturationAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit FS_SaturationAudioProcessorEditor(FS_SaturationAudioProcessor &);
    ~FS_SaturationAudioProcessorEditor() { setLookAndFeel(nullptr); };

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    FS_LookAndFeel fsTheme;

    FS_SaturationAudioProcessor &audioProcessor;

    Label driveLabel;
    Slider driveSlider;
    unique_ptr<SliderAttachment> driveAttachment;

    Label mixLabel;
    Slider mixSlider;
    unique_ptr<SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FS_SaturationAudioProcessorEditor)
};