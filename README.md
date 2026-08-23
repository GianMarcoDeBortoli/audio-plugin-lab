# audio-plugin-lab

My playground for real-time audio-processing plugin development.

---

This repository contains my attempt to learn real-time audio plugin development in **C++** using the **JUCE** framework.

This repository contains **source code only**. Pre-built plugin binaries are **not** included.

Instructions below describe the build process of the binaries.

---

## Requirements

The following tools must be installed **locally**. They are **not** included in this repository.

### Common requirements

- **CMake ≥ 3.15**
- **C++17-compatible compiler**
- **Git** (with submodule support)

### Platform-specific notes

#### macOS
- Xcode Command Line Tools (provides `clang`, SDKs, and `make`)
- Supported plugin formats: **VST3, AU, Standalone**

```bash
xcode-select --install
brew install cmake
```

#### Linux
- gcc or clang
- make
- supported plugin formats: **VST3, Standalone**

```bash
sudo apt install build-essential cmake
```

#### Windows
- Visual Studio Build Toold (MSVC)
- CMake
- Supposted plugin formats: **VST3, Standalone**

---

## Repository structure

- **`src/apl/`**: Custom library, shared by all plugins
- **`tests/`**: unit tests for custom library debug
- **`external/`**: Contains JUCE as a git submodule
- **`plugins/`**: Production-ready plugins
- **`sandbox/`**: Experimental plugins for learning or testing ideas
- **`templates/plugin/`**: Starter plugin template to copy when creating a new plugin
- **`configure.sh`**: One-time configuration script that generates the build system
- **`build.sh`**: Convenience script to build one or all plugins

---

## Access project

1. Clone the repository
    ```bash
        git clone --recurse-submodules https://github.com/GianMarcoDeBortoli/audio-plugin-lab.git
        cd audio-plugin-lab
    ```

2. Configure the project
    ```bash
    ./configure.sh
    ```
    This creates the build/ directory and generates the build system.
    You need to rerun this step if:
    - you delete the `build/` folder
    - you change CMake configurations options

---

## Tests

1. Build a test file
    ```
    cmake --build build --target test_delayline
    ```

2. Run the test file
    - Run a specific test
    ```
    ctest --test-dir build test_file_name
    ```
Or run all built test files
    ```
    ctest --test-dir build
    ```
    or:
    ```
    ctest --test-dir build --output-on-failure
    ```

---

## Build extisting plugins

- Build all plugins:
    ```bash
    ./build.sh all
    ```
    This builds **all plugin targets** (including sandbox plugins) using the `Debug` configuration (default)

    You can also specify the format and jobs:
    ```bash
    ./build.sh all [format] [config] [jobs]
    ```

- Build a single plugin:
    ```bash
    ./build.sh <plugin_name> [format] [config] [jobs]
    ```
    This build only the requested plugin.

See `build.sh` for more details.

---

## Add a new plugin

The folder `templates/new_plugin` contains a **starter JUCE plugin** ready to be copied and customized.

### How to create a new plugin

1. Copy this folder to `plugins/<NEW_PLUGIN_NAME>`.

2. Follow instructions in `plugins/<NEW_PLUGIN_NAME>/CMakeLists.txt`

3. Add the new plugin to the top-level `CMakeLists.txt`:
    ```cmake
    add_subdirectory(plugins/new_plugin_name)
    ```

4. Build with CMake as described above

---

## License

The source code in this repository is licensed under the *MIT license*.