#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

class FS_LookAndFeel : public LookAndFeel_V4 {
public:
    static constexpr uint32 colorBackground = 0xff121418;
    static constexpr uint32 colorPanel = 0xff1a1d24;
    static constexpr uint32 colorAmber = 0xfff59e0b;
    static constexpr uint32 colorAmberGlow = 0x44f59e0b;
    static constexpr uint32 colorTrackDark = 0xff252932;
    static constexpr uint32 colorTextLight = 0xffe2e8f0;

    FS_LookAndFeel() {
        setColour(Slider::textBoxTextColourId, Colour(colorTextLight));
        setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
        setColour(Label::textColourId, Colour(colorAmber));
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider & /*slider*/) override {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 8.0f;
        auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        auto centre = bounds.getCentre();

        juce::Path backgroundArc;
        backgroundArc.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);

        g.setColour(juce::Colour(colorTrackDark));
        g.strokePath(backgroundArc,
                     juce::PathStrokeType(4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        if (sliderPosProportional > 0.0f) {
            juce::Path valueArc;
            valueArc.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, toAngle, true);

            g.setColour(juce::Colour(colorAmberGlow));
            g.strokePath(valueArc,
                         juce::PathStrokeType(8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            g.setColour(juce::Colour(colorAmber));
            g.strokePath(valueArc,
                         juce::PathStrokeType(4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        auto knobRadius = radius - 8.0f;
        auto knobBounds =
            juce::Rectangle<float>(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        juce::ColourGradient knobGradient(juce::Colour(0xff2a2e38), centre.x, centre.y - knobRadius,
                                          juce::Colour(0xff16181e), centre.x, centre.y + knobRadius, false);
        g.setGradientFill(knobGradient);
        g.fillEllipse(knobBounds);

        g.setColour(juce::Colour(0xff383e4c));
        g.drawEllipse(knobBounds, 1.2f);

        juce::Path pointer;
        auto pointerLength = knobRadius * 0.45f;
        auto pointerThickness = 3.0f;
        pointer.addRoundedRectangle(-pointerThickness * 0.5f, -knobRadius + 3.0f, pointerThickness, pointerLength,
                                    1.5f);
        pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(centre.x, centre.y));

        g.setColour(juce::Colour(colorAmber));
        g.fillPath(pointer);
    }
};