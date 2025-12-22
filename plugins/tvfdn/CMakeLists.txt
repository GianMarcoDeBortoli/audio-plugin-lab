juce_add_plugin(tvfdn
    COMPANY_NAME "Gian Plugins"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Time-Varying Feedback Delay Network"
)

juce_generate_juce_header(tvfdn)

target_sources(tvfdn
    PRIVATE
        PluginProcessor.cpp
        PluginEditor.cpp
)

target_link_libraries(tvfdn
    PRIVATE
        juce::juce_audio_utils
)