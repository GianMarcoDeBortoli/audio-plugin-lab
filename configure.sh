#!/bin/bash

# Configure the project
# Creates the build folder and generates Makefiles
set -e
cmake -S . -B build \