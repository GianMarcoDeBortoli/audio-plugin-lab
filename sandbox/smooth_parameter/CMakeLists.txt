# ============================================================
# Sandbox plugin - Smooth Parameter
# ============================================================

juce_add_plugin(smooth_parameter
    COMPANY_NAME        "${PROJECT_COMPANY_NAME}"
    PRODUCT_NAME        "Smooth Parameter"
    BUNDLE_ID           "${PROJECT_BUNDLE_ID_BASE}.sandbox.smooth_parameter"

    IS_SYNTH            FALSE
    NEEDS_MIDI_INPUT    FALSE
    NEEDS_MIDI_OUTPUT   FALSE
    IS_MIDI_EFFECT      FALSE

    FORMATS             VST3 AU Standalone
    
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Generate JuceHeader.h
juce_generate_juce_header(smooth_parameter)

# Plugin source files
target_sources(smooth_parameter
    PRIVATE
        PluginProcessor.cpp
        PluginEditor.cpp
)

# Link dependencies
target_link_libraries(smooth_parameter
    PRIVATE
        dsp
)

# Enforce C++17 explicitly
target_compile_features(smooth_parameter
    PRIVATE
        cxx_std_17
)

target_compile_definitions(smooth_parameter
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