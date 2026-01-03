# ============================================================
# DSP library (JUCE-independent)
# ============================================================

add_library(dsp
    # Add DSP source files here
    SmoothParameter.cpp
    DelayLine.cpp
    # FDN.cpp
    # Matrix.cpp
    # MultichannelAbsorption.cpp
    # MultichannelDelay.cpp
    # OnePoleFilter.cpp
)

# Public include directory for DSP headers
target_include_directories(dsp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# DSP uses Eigen for linear algebra
target_link_libraries(dsp
    PUBLIC
        Eigen3::Eigen
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_dsp
)

# DSP requires C++17
target_compile_features(dsp
    PUBLIC
        cxx_std_17
)