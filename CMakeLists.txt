cmake_minimum_required(VERSION 3.15)

project(JucePlugins LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(external/JUCE)
add_subdirectory(plugins/GainPlugin)