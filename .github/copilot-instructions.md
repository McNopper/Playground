# Copilot Instructions for Playground

This Vulkan 1.4 graphics project implements a modern C++20 SDK for rendering and algorithm experimentation.

## Build, Test, and Lint Commands

### Building

```bash
# Configure (from project root)
cmake --preset=ninja

# Build
cd build
ninja
```

For Visual Studio:
```bash
cmake --preset=vs
# Then open solution in build/ folder
```

### Testing

```bash
# Run all tests
cd bin
PlaygroundSDK_test.exe

# For single test, use gtest filter:
PlaygroundSDK_test.exe --gtest_filter=TestSuiteName.TestName
```

### Code Quality

```bash
# cppcheck (from project root)
cppcheck src --check-level=exhaustive --quiet --std=c++20 --enable=style,performance,portability --suppress=cstyleCast --suppress=useStlAlgorithm

# clang-tidy runs automatically during Ninja builds
```

## High-Level Architecture

**Four-Layer Structure:**

1. **core/** - Foundation layer with no external dependencies
   - `math/` - Vector types (float2, float3, float4), float4x4 matrices
   - `color/` - Color spaces (sRGB, BT709, BT2020) and transfer functions
   - `image/` - Image loading/saving via OpenImageIO
   - `io/` - File I/O utilities
   - `parser/` - String parsing helpers
   - `utility/` - General utilities

2. **cpu/** - CPU-side implementations and algorithms

3. **gpu/** - GPU abstraction layer
   - `shader/` - Multi-language shader compilation (GLSL, SPIR-V, Slang)
   - `vulkan/` - Vulkan wrapper classes
     - `factory/` - Object creation (device, memory, buffers, images)
     - `builder/` - Complex objects via builder pattern (pipelines, descriptor sets)
     - `utility/` - Helper functions and utilities
     - `presentation/` - Window/swapchain management
     - `spirv/` - SPIR-V reflection and analysis
     - `transfer/` - Data transfer utilities

4. **engine/** - High-level rendering framework
   - `renderer/` - Rendering abstractions
   - `runtime/` - Application framework and main loop

**Umbrella Headers:** Each module has a master header (`core/core.h`, `gpu/gpu.h`, `engine/engine.h`) that includes all public headers for that module.

**Dependency Flow:** core → cpu/gpu → engine (strict upward dependencies only)

## Key Conventions

### Naming Patterns

**Files:**
- Match class name exactly: `VulkanDeviceFactory.h` / `.cpp`
- Function-only headers: `snake_case.h` (e.g., `vulkan_helper.h`)

**Types:**
- Classes/Structs: `PascalCase` (e.g., `VulkanDeviceFactory`, `GpuBuffer`)
- Interfaces: Prefix `I` (e.g., `IVulkanWindow`)
- Math types: lowercase+numbers (e.g., `float2`, `float3`, `float4x4`)
- Enums: `enum class` with `PascalCase` names, `SCREAMING_SNAKE_CASE` values

**Variables:**
- Members: `m_snake_case` (e.g., `m_physical_device`, `m_queue_family_index`)
- Local/parameters: `snake_case`
- Global constants: `SCREAMING_SNAKE_CASE`

**Functions:**
- All functions (free and member): `camelCase` (e.g., `createBuffer()`, `getDevice()`)

### Code Style

**Modern C++:**
- Always explicit `std::` - never `using namespace std`
- Brace initialization `{}` for members and constructors
- `= default` and `= delete` for special members
- `const&` for read-only parameters
- `nullptr` not `NULL`
- `auto` when type is obvious
- `enum class` for type safety

**Formatting:**
- 4 spaces, no tabs
- Opening brace on new line for all constructs (functions, classes, control structures)
- Always use braces for control structures, even single statements
- Space after keywords: `if (`, `while (`
- No space after function names: `create()`

**Header Guards:**
- Format: `<NAMESPACE>_<PATH>_<FILENAME>_H_`
- Example: `CORE_MATH_VECTOR_H_`, `GPU_VULKAN_FACTORY_VULKANDEVICEFACTORY_H_`

**Include Order:**
1. Corresponding header (for .cpp files)
2. Standard library
3. Third-party libraries
4. Project headers

### Coordinate System

**Critical for rendering code:**
- **World Space**: Right-handed, Y-up
- **Clip Space/NDC**: Y-down (Vulkan convention: -1 = top, +1 = bottom)
- **Projection matrices**: Must use negative Y-scale to convert Y-up world to Y-down NDC
- **Texture UVs**: Top-left origin (V=0 is top, V=1 is bottom)
- **Images**: Top-left origin (matches PNG/JPEG standard) - never flip
- **Front face**: Counter-clockwise winding
- **Matrices**: Column-major storage

See `docs/coordinate_system.md` for complete details.

### Vulkan Patterns

- Use volk for dynamic Vulkan loading (not static linking)
- Factory classes create simple Vulkan objects (device, buffers, images)
- Builder classes construct complex objects via method chaining (pipelines, descriptor sets)
- SPIRV-Reflect for shader reflection and descriptor set layout generation
- Support multiple shader languages: GLSL, SPIR-V binary, Slang

### Project Organization

- **src/** - SDK library code
- **example01-05/** - Standalone examples demonstrating SDK features
- **test/** - Google Test unit tests
- **resources/** - Shaders and test assets
- **bin/** - All build outputs (executables, libraries)

### Adding New Code

**New Example:**
1. Create `exampleNN/` directory with `main.cpp`
2. Add to `CMakeLists.txt` with `collect_sources_and_headers()` and `add_executable()`
3. Link against `PlaygroundSDK volk::volk_headers unofficial::spirv-reflect`

**New Test:**
1. Add to `test/` directory
2. Use Google Test framework (`#include <gtest/gtest.h>`)
3. CMake auto-discovers via `file(GLOB TEST_SOURCES)`

**New Class:**
1. Place in appropriate `src/` subdirectory matching architecture
2. Add to module's umbrella header (e.g., `core/core.h`)
3. Follow naming conventions exactly

### Dependencies

Managed via vcpkg:
- **Vulkan**: volk (dynamic loader), SPIRV-Tools, SPIRV-Reflect
- **Shaders**: shaderc (GLSL), slang (Slang language)
- **Graphics**: imgui (UI), glfw (windowing), fastgltf (model loading)
- **Images**: OpenImageIO (loading/saving)
- **Testing**: gtest

Requires `VULKAN_SDK` environment variable.
