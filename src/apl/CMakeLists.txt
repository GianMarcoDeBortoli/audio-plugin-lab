add_library(apl
    # connectors/Series.cpp
    # connectors/Series.h
    # connectors/Parallel.cpp
    # connectors/Parallel.h
    # connectors/Recursion.cpp
    # connectors/Recursion.h

    dsp/FFT.cpp
    dsp/FFT.h

    # generators/Waveform.cpp
    # generators/Waveform.h
    # generators/Sinewave.cpp
    # generators/Sinewave.h

    math/constants.h
            
    operators/DelayLine.cpp
    operators/DelayLine.h
    operators/Matrix.cpp
    operators/Matrix.h
    operators/TimeVaryingMatrix.cpp
    operators/TimeVaryingMatrix.h
    # operators/OnePoleFilter.cpp
    # operators/OnePoleFilter.h
    operators/FirstOrderShelf.cpp
    operators/FirstOrderShelf.h
    operators/MultichannelAbsorption.cpp
    operators/MultichannelAbsorption.h
    operators/MultichannelDelay.cpp
    operators/MultichannelDelay.h

    # operators/FDN.cpp
    # operators/FDN.h

    utils/SmoothParameter.cpp
    utils/SmoothParameter.h
)

# Public include directory for apl headers
target_include_directories(apl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Dependencies
target_link_libraries(apl
    PUBLIC
        Eigen3::Eigen
        juce_dsp
        juce_core
)

# apl requires C++17
target_compile_features(apl
    PUBLIC
        cxx_std_17
)