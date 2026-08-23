add_library(apl
    # connectors/Series.cpp
    # connectors/Series.h
    # connectors/Parallel.cpp
    # connectors/Parallel.h
    # connectors/Recursion.cpp
    # connectors/Recursion.h

    # generators/Waveform.cpp
    # generators/Waveform.h
    # generators/Sinewave.cpp
    # generators/Sinewave.h
            
    operators/DelayLine.cpp
    operators/DelayLine.h
    # operators/FDN.cpp
    # operators/FDN.h
    # operators/Matrix.cpp
    # operators/Matrix.h
    # operators/MultichannelAbsorption.cpp
    # operators/MultichannelAbsorption.h
    # operators/MultichannelDelay.cpp
    # operators/MultichannelDelay.h
    # operators/OnePoleFilter.cpp
    # operators/OnePoleFilter.h

    utils/SmoothParameter.cpp
    utils/SmoothParameter.h
)

# Public include directory for DSP headers
target_include_directories(apl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# DSP uses Eigen for linear algebra
target_link_libraries(apl
    PRIVATE
        Eigen3::Eigen
)

# DSP requires C++17
target_compile_features(apl
    PUBLIC
        cxx_std_17
)