#pragma once
#include <cmath>

class SaturationEngine {
private:
    double currentSampleRate = 44100.0;
    float driveGain = 1.0f;
    float lastDriveDb = -1.0f;
    float autoGainComp = 1.0f;
    float mix = 1.0f;

    float dcBlockerR = 0.995;

    float x1[2] = {0.0f, 0.0f};
    float y1[2] = {0.0f, 0.0f};

public:
    SaturationEngine() = default;

    void prepare(double sampleRate) noexcept {
        currentSampleRate = sampleRate;
        constexpr float cutoffHz = 15.0f;
        constexpr float twoPi = 6.28318530717958647692f;

        dcBlockerR = 1.0f - (twoPi * cutoffHz / static_cast<float>(sampleRate));

        reset();
    }

    void reset() noexcept {
        // x1[0] = x1[1] = 0.0f;
        // y1[0] = y1[1] = 0.0f;

        std::fill(std::begin(x1), std::end(x1), 0.0f);
        std::fill(std::begin(y1), std::end(y1), 0.0f);
    }

    inline float processSample(float input, int channel) noexcept {
        const float dry = input;
        const float driven = input * driveGain;

        // assymetric saturation
        const float shaped = driven + 0.15f * (driven * driven);
        float wet = std::tanhf(shaped);

        // DC-Blocker (filter)
        const float x0 = wet;
        wet = x0 - x1[channel] + dcBlockerR * y1[channel];
        x1[channel] = x0;
        y1[channel] = wet;

        wet *= autoGainComp;

        return (dry * (1.0f - mix)) + (wet * mix);
    }

    void processBlock(float *left, float *right, int numSamples) noexcept {
        for (int i = 0; i < numSamples; ++i) {
            left[i] = processSample(left[i], 0);
            right[i] = processSample(right[i], 1);
        }
    }

    void setDrive(float driveDb) noexcept {
        if (driveDb < 1.0f)
            driveDb = 1.0f;

        if (driveDb != lastDriveDb) {
            driveGain = std::pow(10.0f, driveDb / 20.0f);
            autoGainComp = 1.0f / std::sqrt(driveGain);
            lastDriveDb = driveDb;
        }
    }

    void setMix(float mixFraction) noexcept { mix = std::clamp(mixFraction, 0.0f, 1.0f); }
};