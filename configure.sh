#!/bin/bash

# Configure the project
# Creates the build folder and generates Makefiles
set -e # Exit the script immediately if any command returns a non-zero exit status.
cmake -S . -B build \
echo "Configuration successful"