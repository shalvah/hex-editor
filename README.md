# Hex Editor

A standalone hexadecimal editor built with C++ and Qt 6.

## Running the App

You don't need to install Qt or any development tools to run the application.

1. Go to the **Releases** page on GitHub.
2. Download the latest release `.zip` file for Windows.
3. **Important:** Extract the entire `.zip` file to a folder on your computer. (Do not run the executable directly from inside the compressed ZIP previewer, or it will fail to load the required DLLs).
4. Double-click `hex-editor.exe` to start the application.

---

## Development

If you want to modify the code or build the project from scratch, follow these steps.

### Prerequisites

*   **Qt 6:** (Specifically requires the `Core` and `Widgets` components).
*   **CMake:** (Version 3.19 or higher).
*   **C++ Compiler:** (e.g., MinGW-w64 on Windows, MSVC, GCC, or Clang).

### Building with Qt Creator (Recommended)

1. Open Qt Creator.
2. Go to **File > Open File or Project...** and select the `CMakeLists.txt` file in the root directory.
3. Configure the project by selecting a compatible Kit (e.g., Desktop Qt 6 MinGW 64-bit).
4. Click the **Build** (hammer icon) or **Run** (green play button) in the bottom-left corner.

### Building from the Command Line (Windows MinGW)

1. Open the Qt Command Prompt from your Start menu (e.g., "Qt 6.10.2 (MinGW 64-bit)").
2. Navigate to the project root directory:
   ```cmd
   cd path\to\hex-editor
   ```
3. Configure the project for a Release build:
   ```cmd
   cmake -B build/release -S . -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"
   ```
4. Compile the project:
   ```cmd
   cmake --build build/release
   ```

### Creating a Standalone Release ZIP

If you want to package the application to share with others:
```cmd
# Navigate to your build directory
cd build/release

# Run the deployment tool to copy required Qt and compiler DLLs
windeployqt --compiler-runtime hex-editor.exe
```
After running this, you can ZIP the entire `build/release` folder.

---

## Running Tests

This project includes automated tests to ensure functionality (located in the `hex-editor-tests` directory).

To run the tests from the command line after building the project:

```cmd
cd build/release
ctest --output-on-failure
```

Alternatively, if you are using Qt Creator, you can select the test executable from the Run configuration menu and run it directly.
