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

**Zero warnings policy:** All code must compile without warnings and produce no cppcheck diagnostics. Treat warnings as errors.

```bash
# cppcheck (from project root)
cppcheck src --check-level=exhaustive --quiet --std=c++20 --enable=style,performance,portability --suppress=cstyleCast --suppress=useStlAlgorithm

# clang-tidy runs automatically during Ninja builds
```

## High-Level Architecture

**Four-Layer Structure:**

1. **core/** - Foundation layer with no external dependencies
   - `math/` - Vector types (float2–float7), matrix types (float2x2–float7x7)
   - `color/` - Color spaces (sRGB, BT709, BT2020) and transfer functions
   - `image/` - Image loading/saving via OpenImageIO
   - `io/` - File I/O utilities
   - `parser/` - String parsing helpers
   - `templates/` - Template utilities (e.g., Filter)
   - `utility/` - General utilities (base64 encoding, gzip compression)

2. **cpu/** - CPU-side implementations and algorithms
   - `ai/` - AI/ML components (activation functions, loss functions, MLP)
   - `geometry/` - Procedural mesh generation (cube, sphere, cone, torus)

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
   - `asset/` - Asset loading and parsing (future: glTF, materials, etc.)
   - `renderer/` - Rendering abstractions
     - `geometry/` - Abstract geometry base and primitive types
       - `curve/` - Curve primitives
       - `point/` - Point primitives
       - `surface/` - Surface primitives (e.g., TriangleMesh)
       - `volume/` - Volume primitives
     - `material/` - Material definitions (inspired by OpenUSD/MaterialX)
       - `pbr/` - PBR material implementations (future)
       - `splat/` - Splatting shader implementations (future)
     - `scene/` - Scene objects (Renderable combines geometry + material + transform)
     - `backend/` - Backend-specific implementations
       - `common/buffer/` - GPU buffer abstractions (uniform, vertex, index, storage, descriptor)
       - `common/image/` - Texture abstractions (Texture2D, TextureCube, Sampler)
       - `rasterizer/` - Rasterizer backend (future)
       - `raytracer/` - Raytracer backend (future)
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
- Math types: lowercase+numbers (e.g., `float2`, `float3`, `float4`, `float5`, `float6`, `float7`, `float4x4`)
- Enums: `enum class` with `PascalCase` names, `SCREAMING_SNAKE_CASE` values

**Variables:**
- Members: `m_snake_case` (e.g., `m_physical_device`, `m_queue_family_index`)
- Local/parameters: `snake_case`
- Global constants: `SCREAMING_SNAKE_CASE`

**Functions:**
- All functions (free and member): `camelCase` (e.g., `createBuffer()`, `getDevice()`)

### Code Style

**Modern C++:**
- Always explicit `std::` for all standard library items - never `using namespace std`
  - Types: `std::vector`, `std::string`, `std::uint32_t`
  - Functions: `std::sqrt()`, `std::sin()`, `std::cos()`, `std::pow()`
- Smart pointers for ownership:
  - `std::unique_ptr<T>` for exclusive ownership (factory returns, owned resources)
  - `std::shared_ptr<T>` for shared ownership (materials, textures, geometry shared across objects)
  - Pass `shared_ptr` parameters by `const std::shared_ptr<const T>&` (avoids atomic refcount overhead)
  - Raw pointers acceptable ONLY for: Vulkan handles (VkDevice, etc.), GLFW handles (GLFWwindow*), non-owning views
- Brace initialization `{}` for member variables and local constants
- `= default` and `= delete` for special members
- `const&` for read-only parameters
- `nullptr` not `NULL`
- `auto` when type is obvious
- `enum class` for type safety

**Error Handling:**
- Use `std::optional<T>` for functions that may fail or return nothing
- Use `bool` return for `create()` / initialization methods
- No exceptions in production code
- No logging library — return values indicate success/failure

**Class Structure:**
- Access specifier order: `private:` first (member variables), then `public:` (constructors, methods)
- Delete default and copy constructors for resource/factory classes
- Virtual destructors on base classes
- Non-copyable pattern for GPU resource wrappers:
  ```cpp
  class GpuBuffer
  {
  private:
      VkDevice m_device{ VK_NULL_HANDLE };

  public:
      GpuBuffer() = delete;
      GpuBuffer(const GpuBuffer&) = delete;
      GpuBuffer& operator=(const GpuBuffer&) = delete;

      explicit GpuBuffer(VkDevice device);
      virtual ~GpuBuffer();
  };
  ```

**Namespace Usage:**
- Minimal — mostly global or class scope
- No deep hierarchical namespaces (e.g., no `gpu::vulkan::`)
- Exception: `ebnf` namespace for parser utilities
- Anonymous namespaces in .cpp files and test files for internal helpers

**Initialization style:**
- **Member variables**: Always use `{}` brace initialization
  ```cpp
  VkDevice m_device{ VK_NULL_HANDLE };
  bool m_enabled{ false };
  std::string m_name{};
  ```
- **Local variables with constants/literals**: Use `{}` brace initialization
  ```cpp
  uint32_t count{ 0u };
  bool result{ false };
  VkDescriptorSetLayout layout{ VK_NULL_HANDLE };
  ```
- **Local variables from function calls**: Use `=` assignment
  ```cpp
  auto physical_devices = gatherPhysicalDevices(instance);
  VkDevice device = factory.create();
  ```
- **Default function parameters**: Use `=` (standard C++ syntax)
  ```cpp
  void function(uint32_t offset = 0u, bool enabled = true);
  ```
- **Assignment statements**: Use `=`
  ```cpp
  attr.buffer = buffer;
  m_count = getCount();
  ```

**Formatting:**
- 4 spaces, no tabs
- Opening brace on new line for all constructs (functions, classes, control structures)
- Always use braces for control structures, even single statements
- Space after keywords: `if (`, `while (`
- No space after function names: `create()`
- Space around operators: `x = y + z`
- Constructor initializer lists: Colon on same line as closing paren, pack initializers on current line if they fit, otherwise one per line
  ```cpp
  // Short initializer list - pack on single line
  ClassName::ClassName(Type param1, Type param2) :
      m_member1{ param1 }, m_member2{ param2 }, m_member3{}
  {
  }

  // Long initializer list - one per line
  ClassName::ClassName(Type param1, Type param2, Type param3, Type param4) :
      m_member1{ param1 },
      m_member2{ param2 },
      m_member3{ param3 },
      m_member4{ param4 }
  {
  }
  ```

**Header Guards:**
- Format: `<NAMESPACE>_<PATH>_<FILENAME>_H_`
- Example: `CORE_MATH_VECTOR_H_`, `GPU_VULKAN_FACTORY_VULKANDEVICEFACTORY_H_`

**Include Order** (separate each group with a blank line):
1. Corresponding header (for .cpp files)
2. Standard library
3. Third-party libraries
4. Project headers

```cpp
#include "vector.h"

#include <cmath>

#include <volk.h>

#include "gpu/gpu.h"
```

**Comments:**
- Write self-documenting code - minimize comments
- Comment "why", not "what"
- Use `//` for single-line comments
- Use blank `//` lines to separate logical sections

**Commit Messages:**
- Use present tense ("Add feature" not "Added feature")
- Start with a verb ("Fix", "Add", "Update", "Refactor")
- Keep first line under 72 characters

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

### Shader Conventions

- Primary language: Slang (`.slang` files, `snake_case` naming)
- Secondary: GLSL (`.vert`/`.frag`) and SPIR-V binary (`.spirv`)
- Uniform variables: `u_` prefix with camelCase (e.g., `u_worldMatrix`, `u_projectionMatrix`)
- Vertex inputs: `i_` prefix (e.g., `i_position`, `i_texcoord`)
- Shader functions: `camelCase`
- Shader variables/parameters: `camelCase` (MaterialX convention)

### Test Conventions

- File naming: `{module}_{feature}_test.cpp` (e.g., `core_math_test.cpp`, `gpu_vulkan_test.cpp`)
- Test suites: `TEST(TestCategory, TestName)` with PascalCase names
- Anonymous namespaces for test helper structs/functions

### Project Organization

- **src/** - SDK library code
- **example01-06/** - Standalone examples demonstrating SDK features
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
- **Shaders**: shaderc (GLSL via vcpkg), slang (included with Vulkan SDK)
- **Graphics**: imgui (UI), glfw (windowing)
- **Images**: OpenImageIO (loading/saving)
- **Data**: nlohmann/json (JSON parsing), zlib (gzip compression)
- **Testing**: gtest

Requires `VULKAN_SDK` environment variable.
