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

# Build universal binaries (Intel + Apple Silicon)
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
endif()

# ------------------------------------------------------------
# Project-wide metadata (used by plugins)
# ------------------------------------------------------------

set(PROJECT_COMPANY_NAME "Gian Plugins")
set(PROJECT_BUNDLE_ID_BASE "com.gian.audiopluginlab")

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
    add_subdirectory(sandbox/juce-tutorial-1)
endif()