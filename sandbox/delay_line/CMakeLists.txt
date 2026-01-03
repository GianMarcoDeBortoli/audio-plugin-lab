# ============================================================
# Sandbox plugin - Delay Line
# ============================================================

# JUCE plugin target
juce_add_plugin(delay_line
    COMPANY_NAME        "${PROJECT_COMPANY_NAME}"
    PRODUCT_NAME        "Delay Line"
    BUNDLE_ID           "${PROJECT_BUNDLE_ID_BASE}.delay_line"

    IS_SYNTH            FALSE
    NEEDS_MIDI_INPUT    FALSE
    NEEDS_MIDI_OUTPUT   FALSE
    IS_MIDI_EFFECT      FALSE

    # Minimal plugin formats for quick iteration
    FORMATS             VST3 AU Standalone

    # Automatically copy the plugin after building
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Generate JuceHeader.h for this plugin
juce_generate_juce_header(delay_line)

# Plugin source files — replace or add more as needed
target_sources(delay_line
    PRIVATE
        PluginProcessor.cpp
        PluginEditor.cpp
)

# Link libraries: JUCE + DSP + Eigen
target_link_libraries(delay_line
    PRIVATE
        dsp
)

# Ensure C++17
target_compile_features(delay_line
    PRIVATE
        cxx_std_17
)

target_compile_definitions(delay_line
    PRIVATE
        JUCE_ASIO=1
        JUCE_DIRECTSOUND=0
        JUCE_USE_FLAC=0
        JUCE_USE_OGGVORBIS=0
        JUCE_USE_WINDOWS_MEDIA_FORMAT=0
        JUCE_WEB_BROWSER=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
        ${windows_defines}
        ${linux_defines}
)