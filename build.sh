#!/bin/bash

# Usage:
# ./build.sh <plugin_name> [format] [config] [jobs]
# Example:
# ./build.sh tvfdn VST3 Debug 4

# Plugin target
target=${1}

# Format: VST3/AU/Standalone (default: Standalone)
format=${2:-Standalone}

# Build config: Debug/Release (default: Debug)
config=${3:-Debug}

# Number of parallel jobs (default: 4)
jobs=${4:-4}

# Build command
cmake --build build --target ${target}_${format} --config ${config} --parallel ${jobs}