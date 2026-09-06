# ============================================================
# TVFDN — production JUCE plugin
# ============================================================

juce_add_plugin(tvfdn
    COMPANY_NAME            "${PROJECT_COMPANY_NAME}"
    PRODUCT_NAME            "TVFDN"
    BUNDLE_ID               "${PROJECT_BUNDLE_ID_BASE}.tvfdn"

    IS_SYNTH                FALSE
    NEEDS_MIDI_INPUT        FALSE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE

    FORMATS                 VST3 AU Standalone

    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Generate JuceHeader.h
juce_generate_juce_header(tvfdn)

# Plugin source files
target_sources(tvfdn
    PRIVATE
        ParameterManager.cpp
        ParameterManager.h
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h
)

# Link dependencies
target_link_libraries(tvfdn
    PRIVATE
        juce::juce_audio_utils
        apl
)

# Enforce C++17 explicitly
target_compile_features(tvfdn
    PRIVATE
        cxx_std_17
)

target_compile_definitions(tvfdn
    PRIVATE
        JUCE_VST3_CAN_REPLACE_VST2=0
)