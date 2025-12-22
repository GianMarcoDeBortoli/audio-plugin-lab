# audio-plugin-lab

My playground for real-time audio-processing plugin development.

---

This repository contains my attempt to learn real-time audio plugin development in **C++** using the **JUCE** framework.

This repository contains **source code only**. Pre-built plugin binaries are **not** included.

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

## License

The source code in this repository is licensed under the *MIT license*.
