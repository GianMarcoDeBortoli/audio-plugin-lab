# ============================================================
# Plugin Template — copy this folder to create a new plugin
# ============================================================

# -------------------------------
# Instructions:
# 1. Copy this folder to 'plugins/<NEW_PLUGIN_NAME>'
# 2. Replace <PLUGIN_NAME> with your new plugin target name
# 3. Replace <PRODUCT_DISPLAY_NAME> with the user-facing plugin name
# 4. Modify targets according to needs
# -------------------------------

# JUCE plugin target
juce_add_plugin(<PLUGIN_NAME>
    COMPANY_NAME        "${PROJECT_COMPANY_NAME}"
    PRODUCT_NAME        "<PRODUCT_DISPLAY_NAME>"
    BUNDLE_ID           "${PROJECT_BUNDLE_ID_BASE}.<PLUGIN_NAME>"

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
juce_generate_juce_header(<PLUGIN_NAME>)

# Plugin source files — replace or add more as needed
target_sources(<PLUGIN_NAME>
    PRIVATE
        PluginProcessor.cpp
        PluginEditor.cpp
)

# Link libraries: JUCE + DSP + Eigen
target_link_libraries(<PLUGIN_NAME>
    PRIVATE
        dsp
)

# Ensure C++17
target_compile_features(<PLUGIN_NAME>
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