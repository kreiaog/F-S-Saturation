cmake_minimum_required(VERSION 3.22)

project(F_S_Saturation VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SUBMODULES_RECURSE TRUE
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(F_S_Saturation
    COMPANY_NAME "KreiaDSP"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Krei
    PLUGIN_CODE Sat1
    FORMATS VST3 Standalone
    PRODUCT_NAME "F-S-Saturation"
)

target_sources(F_S_Saturation PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/DSP/SaturationEngine.h
    Source/GUI/FS_LookAndFeel.h
)

target_link_libraries(F_S_Saturation PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

target_include_directories(F_S_Saturation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_compile_definitions(F_S_Saturation PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)