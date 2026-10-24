#include "PluginEditor.h"

#include "PluginProcessor.h"

FS_SaturationAudioProcessorEditor::FS_SaturationAudioProcessorEditor(FS_SaturationAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    setSize(400, 300);

    setLookAndFeel(&fsTheme);

    // drive ui
    driveLabel.setText("Drive", dontSendNotification);
    driveLabel.setJustificationType(Justification::centred);
    driveLabel.attachToComponent(&driveSlider, false);
    addAndMakeVisible(driveLabel);

    driveSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    driveSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(driveSlider);
    driveAttachment = make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drive", driveSlider);

    // mix ui
    mixLabel.setText("Mix", dontSendNotification);
    mixLabel.setJustificationType(Justification::centred);
    mixLabel.attachToComponent(&mixSlider, false);
    addAndMakeVisible(mixLabel);

    mixSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    mixSlider.setTextValueSuffix(" %");
    addAndMakeVisible(mixSlider);
    mixAttachment = make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "mix", mixSlider);

    // oversampling UI
    oversamplingLabel.setText("Oversampling", juce::dontSendNotification);
    oversamplingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(oversamplingLabel);

    oversamplingBox.addItem("Off", 1);
    oversamplingBox.addItem("2x", 2);
    oversamplingBox.addItem("4x", 3);
    oversamplingBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(oversamplingBox);

    oversamplingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "oversampling", oversamplingBox);
}

void FS_SaturationAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(Colour(FS_LookAndFeel::colorBackground));

    auto panelBounds = getLocalBounds().reduced(12).toFloat();
    g.setColour(Colour(FS_LookAndFeel::colorPanel));
    g.fillRoundedRectangle(panelBounds, 8.0f);
    g.setColour(Colour(0xff2a2e39));
    g.drawRoundedRectangle(panelBounds, 8.0f, 1.5f);

    g.setColour(Colour(FS_LookAndFeel::colorAmber));
    g.setFont(Font(18.0f, Font::bold));
    g.drawFittedText("F - S   S A T U R A T I O N", 0, 24, getWidth(), 24, juce::Justification::centred, 1);
}

void FS_SaturationAudioProcessorEditor::resized() {
    const int dialSize = 100;
    const int yPos = 80;

    driveSlider.setBounds(60, yPos, dialSize, dialSize);
    mixSlider.setBounds(getWidth() - 60 - dialSize, yPos, dialSize, dialSize);

    const int boxWidth = 140;
    const int boxHeight = 24;
    const int boxY = 225;

    oversamplingLabel.setBounds((getWidth() - boxWidth) / 2, boxY - 22, boxWidth, 20);
    oversamplingBox.setBounds((getWidth() - boxWidth) / 2, boxY, boxWidth, boxHeight);
}