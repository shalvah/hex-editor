# Hex Editor

## Prerequisites

To compile and run this application, you will need a C++17 compiler, **CMake** (v3.19+), and **Qt 6** installed on your system. You can install all these with the [Qt online installer](https://doc.qt.io/qt-6/qt-online-installation.html). Ensure **CMake** and your selected compiler (e.g. MinGW 64-bit) are checked under the "Developer and Designer Tools" section.

If you already have Qt installed, you can use the [Qt Maintenance Tool](https://doc.qt.io/qtinstallerframework/ifw-use-cases-add.html) to add CMake and a compiler, or install them from an external source.

---

## Building and running the application

### Windows

1. Open **Qt Command Prompt** (search "Qt" in Start menu — it's usually named "Qt 6.x.x (MinGW...)")
2. Navigate to the project directory and run:

```bat
:: Generate build configuration files
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"
:: Build the executable
cmake --build build
::Copy all required Qt DLLs into the build folder
windeployqt build\hex-editor.exe
:: Run the executable
build\hex-editor.exe
```

**Note:** If you installed `cmake` via Qt, it may not be found in your system PATH. Try replacing `cmake` with the full path, usually `C:\Qt\Tools\CMake_64\bin\cmake.exe`.

**Note:** The `-G "MinGW Makefiles"` argument assumes you chose MinGW as the Qt compiler. If you chose something else (eg MSVC), change the argument (eg to `-G "NMake Makefiles"`), then run `del build/CMakeCache.txt` to delete the build cache, then try again.

### macOS

1. Ensure Qt is installed (via [qt.io](https://qt.io) or `brew install qt`)
2. If installed via Homebrew, add Qt to your PATH first:
```bash
export PATH="/opt/homebrew/opt/qt/bin:$PATH"
```
3. Build and run:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
macdeployqt build/hex-editor
build/hex-editor
```

### Linux

1. Install Qt and build tools, e.g. on Ubuntu/Debian:
```bash
sudo apt install qt6-base-dev cmake make g++
```
2. Build and run:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
build/hex-editor
```

---

**Note:** These are debug builds. For a release build, replace `Debug` with `Release` in the cmake configure step.


## Running Tests

This project includes automated tests for various components built with the Qt Test framework. You can run all tests using CTest (bundled with CMake).

```bat
::Navigate to the `build` directory
cd build
ctest --output-on-failure
```
   
**Note:** As with `cmake`, you may need to use the full path for ctest (`C:\Qt\Tools\CMake_64\bin\ctest.exe`).
