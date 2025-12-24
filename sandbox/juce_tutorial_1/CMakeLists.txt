# ============================================================
# JUCE Tutorial 1 — sandbox plugin
# ============================================================

juce_add_plugin(juce_tutorial_1
    COMPANY_NAME        "${PROJECT_COMPANY_NAME}"
    PRODUCT_NAME        "JUCE Tutorial 1"
    BUNDLE_ID           "${PROJECT_BUNDLE_ID_BASE}.sandbox.juce_tutorial_1"

    IS_SYNTH            FALSE
    NEEDS_MIDI_INPUT    FALSE
    NEEDS_MIDI_OUTPUT   FALSE
    IS_MIDI_EFFECT      FALSE

    FORMATS             VST3 AU Standalone
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Generate JuceHeader.h
juce_generate_juce_header(juce_tutorial_1)

# Plugin source files
target_sources(juce_tutorial_1
    PRIVATE
        MainComponent.cpp
)

# Link dependencies
target_link_libraries(juce_tutorial_1
    PRIVATE
        juce::juce_audio_utils
        dsp
        Eigen3::Eigen
)

# Enforce C++17 explicitly
target_compile_features(juce_tutorial_1
    PRIVATE
        cxx_std_17
)