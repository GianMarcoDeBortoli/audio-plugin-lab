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

- **`dsp/`**: Custom DSP library, shared by all plugins.
- **`external/`**: Contains JUCE as a git submodule.
- **`plugins/`**: Production-ready plugins.
- **`sandbox/`**: Experimental plugins for learning or testing ideas; optional and not intended for production.
- **`templates/plugin/`**: Starter plugin template to copy when creating a new plugin.

---

## Building extisting plugins

To build the plugins currently in this repository, follow these steps:

1. Close the repository with submodules (to get JUCE):
    ```bash
    git clone --recurse-submodules https://github.com/GianMarcoDeBortoli/audio-plugin-lab.git
    cd audio-plugin-lab
    ```
    If you already cloned the repository without submodules, you can add them manually:
    ```bash
    git submodule update --init --recursive
    ```

2. Create a build directory:
    ```bash
    mkdir build
    cd build
    ```

3. Build all plugins:
    ```bash
    cmake ..
    make
    ```
    The first line will configure the build and generate the Makefiles for your platform for all plugins.
    The second line will build all plugins in `plugins/` folder and all plugins in `sandbox/` folder.
    On windows with Visual Studio you can open the generated `.sln` file and build from there.

4. Build only specific plugins:
    ```bash
    cmake -DBUILD_SANDBOX=OFF ..
    make tvfdn
    ```
    The first line will configure the build and generate the Makefiles for your platform for only the plugins in the folder `plugins/`.
    The second line will build only the plugin `plugins/tvfdn/`.

---

## Adding a new plugin

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
