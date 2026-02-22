# Build Instructions

## Build Steps

### Prerequisites

- **CMake**: Version 3.25 or later
- **Vulkan SDK**: Latest version with `VULKAN_SDK` environment variable set
- **C++ Compiler**: 
  - Visual Studio 2022+ (Windows)
  - GCC 11+ or Clang 14+ (Linux)
- **Build System**:
  - Visual Studio 2022+ (includes Ninja)
  - Ninja (standalone for Linux or command-line builds)
- **vcpkg**: Integrated for dependency management

### 1. Open Developer Command Prompt

- **Windows**: Open "Developer Command Prompt for Visual Studio"
- **Linux**: Open a terminal with development tools in PATH

### 2. Navigate to Project Root

```bash
cd Playground
```

### 3. Configure Build

#### Visual Studio Solution (Windows)

```bash
cmake --preset=vs
```

This generates a Visual Studio 2022 solution in the `build` folder.

#### Ninja Build (Windows/Linux)

**Release Build:**
```bash
cmake --preset=ninja
```

**Debug Build:**
```bash
cmake --preset=ninja -DCMAKE_BUILD_TYPE=Debug
```

### 4. Build the Project

#### Visual Studio

1. Open the solution file in the `build` folder
2. Build from within Visual Studio (Ctrl+Shift+B)

#### Ninja

```bash
cd build
ninja
```

## Output

Build artifacts are placed in the `bin` folder:
- Executables: `bin/`
- Libraries: `bin/`

## Updating Dependencies

To update vcpkg baseline for all configurations:

```bash
vcpkg x-update-baseline
```

## Code Quality Checks

### cppcheck

Run static analysis:

```bash
cppcheck src --check-level=exhaustive --quiet --std=c++20 --enable=style,performance,portability --suppress=cstyleCast --suppress=useStlAlgorithm
```

### clang-tidy

Automatically runs during Ninja builds (configured in CMakeLists.txt).

For Visual Studio: [Using Clang-Tidy in Visual Studio](https://learn.microsoft.com/en-us/cpp/code-quality/clang-tidy)

## Troubleshooting

### Vulkan SDK Not Found

Ensure the `VULKAN_SDK` environment variable is set and points to your Vulkan SDK installation.

### vcpkg Package Errors

Try clearing the vcpkg cache:
```bash
vcpkg remove --outdated
vcpkg integrate install
```

### Build Fails with C++20 Errors

Verify your compiler supports C++20:
- Visual Studio 2022 or later
- GCC 11+ or Clang 14+

## Third-Party Libraries

- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [gtest](https://vcpkg.io/en/package/gtest)
- [nlohmann/json](https://github.com/nlohmann/json)
- [OpenImageIO](https://github.com/AcademySoftwareFoundation/OpenImageIO)
- [Shaderc](https://github.com/google/shaderc)
- [slang](https://github.com/shader-slang/slang) *(included with Vulkan SDK)*
- [SPIRV-Reflect](https://github.com/KhronosGroup/SPIRV-Reflect)
- [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools)
- [volk](https://github.com/zeux/volk)
- [zlib](https://github.com/madler/zlib)
