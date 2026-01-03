# ============================================================
# AudioPluginLab — top-level CMake configuration
# ============================================================

cmake_minimum_required(VERSION 3.15)

# ------------------------------------------------------------
# Project definition
# ------------------------------------------------------------

project(AudioPluginLab
    VERSION 0.1.0
    LANGUAGES C CXX
)

# ------------------------------------------------------------
# Project-wide settings
# ------------------------------------------------------------

# Require C++17 everywhere unless explicitly overridden
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required for shared libraries (JUCE plugins)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------------------------------------
# macOS configuration
# ------------------------------------------------------------

if(APPLE)
    # Build universal binaries (Intel + Apple Silicon)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    # Set macOS target deployment version
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
    # Xcode 15 linker workaround
    # If you are using Link Time Optimisation (LTO), the new linker introduced in Xcode 15 may produce a broken binary.
    # As a workaround, add either '-Wl,-weak_reference_mismatches,weak' or '-Wl,-ld_classic' to your linker flags.
    # Once you've selected a workaround, you can add JUCE_SILENCE_XCODE_15_LINKER_WARNING to your preprocessor definitions to silence this warning.
    set(xcode_15_linker -Wl,-ld_classic)
endif()

# Some windows specific defines
if (WIN32)
    set(windows_defines _USE_MATH_DEFINES WIN32_LEAN_AND_MEAN)
endif()

# Some Linux specific defines (libcurl does not link for whatever reason)
if (LINUX)
    set(linux_defines JUCE_USE_CURL=0 JUCE_JACK=1)
endif()

# ------------------------------------------------------------
# Project-wide metadata (used by plugins)
# ------------------------------------------------------------

set(PROJECT_COMPANY_NAME "AudioPluginLab")
set(PROJECT_COMPANY_CODE "APL")
set(PROJECT_BUNDLE_ID_BASE "com.audiopluginlab")

# ------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------

include(FetchContent)

# --- Eigen (header-only linear algebra library) ---
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
)

FetchContent_MakeAvailable(eigen)

# --- JUCE ---
# JUCE is provided as a git submodule under external/JUCE
add_subdirectory(external/JUCE)

# ------------------------------------------------------------
# Internal libraries
# ------------------------------------------------------------

add_subdirectory(dsp)

# ------------------------------------------------------------
# Build options
# ------------------------------------------------------------

option(BUILD_SANDBOX "Build experimental sandbox targets" ON)

# ------------------------------------------------------------
# Plugins
# ------------------------------------------------------------

# Production plugins
add_subdirectory(plugins/tvfdn)

# Experimental / learning code
if(BUILD_SANDBOX)
    add_subdirectory(sandbox/smooth_parameter)
    add_subdirectory(sandbox/delay_line)
endif()