# Contributing to Playground

Thank you for your interest in contributing to Playground! This document provides guidelines for contributing to this Vulkan 1.4 project.

## Getting Started

### Prerequisites

- **C++ Compiler**: Visual Studio 2022+ (Windows) or GCC 11+/Clang 14+ (Linux)
- **CMake**: Version 3.25 or later
- **Vulkan SDK**: Latest version with `VULKAN_SDK` environment variable set
- **vcpkg**: Integrated for dependency management

### Setting Up Your Development Environment

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/Playground.git
   cd Playground
   ```

2. Configure the build:
   ```bash
   cmake --preset=ninja
   ```

3. Build the project:
   ```bash
   cd build
   ninja
   ```

See [Build Instructions](BUILD.md) for detailed setup information.

## Code Style

This project follows strict C++20 coding standards with the following conventions:

### Naming Conventions

**Files:**
- Header files: `.h` extension
- Source files: `.cpp` extension
- Match class name: `VulkanDeviceFactory.h` for class `VulkanDeviceFactory`
- Use snake_case for function-only headers: `vulkan_helper.h`

**Types:**
- Classes/Structs: `PascalCase` (e.g., `VulkanDeviceFactory`, `UniformViewData`)
- Interfaces: Prefix with `I` (e.g., `IVulkanWindow`)
- Math types: lowercase with numbers (e.g., `float2`, `float3`, `float4`, `float4x4`)
- Enums: `PascalCase` with `enum class` (e.g., `ColorSpace`, `TransferFunction`)
- Enum values: `SCREAMING_SNAKE_CASE` (e.g., `ColorSpace::SRGB`, `TransferFunction::LINEAR`)

**Variables:**
- Member variables: `m_snake_case` (e.g., `m_physical_device`, `m_queue_family_index`)
- Local variables: `snake_case` (e.g., `start_pos`, `queue_priorities`)
- Function parameters: `snake_case` (e.g., `physical_device`, `delta_time`)
- Global constants: `SCREAMING_SNAKE_CASE` (e.g., `COLOR_PRIMARY_SRGB`, `D65`)

**Functions:**
- Free functions: `camelCase` (e.g., `beforeString()`, `replaceAll()`)
- Member functions: `camelCase` (e.g., `getEnabledExtensionNames()`, `createBuffer()`)

### Formatting

**Indentation:**
- 4 spaces (no tabs)
- Opening brace on new line for all constructs (functions, classes, control structures)
- Always use braces for control structures, even single statements

```cpp
// Control structures - always use braces
if (position != std::string::npos)
{
    return source.substr(0, position);
}

// Functions and classes
void myFunction()
{
    // implementation
}
```

**Spacing:**
- Space after keywords: `if (`, `while (`, `for (`
- No space between function name and parenthesis: `create()`
- Space around operators: `x = y + z`

**Header Guards:**
- Format: `<NAMESPACE>_<PATH>_<FILENAME>_H_`
- Examples: `CORE_MATH_VECTOR_H_`, `GPU_VULKAN_FACTORY_VULKANDEVICEFACTORY_H_`

**Include Order:**
1. Corresponding header (for .cpp files)
2. Standard library headers
3. Third-party library headers
4. Project headers

```cpp
#include "vector.h"

#include <cmath>

#include <volk.h>

#include "gpu/gpu.h"
```

### Modern C++ Best Practices

- **Always use explicit `std::`** for all standard library items - never `using namespace std`
  - Types: `std::vector`, `std::string`, `std::uint32_t`
  - Functions: `std::sqrt()`, `std::sin()`, `std::cos()`, `std::pow()`
- **Initialization**: Use brace initialization `{ }` for members and constructors
- **Constructors**: Use `= default` and `= delete` appropriately
- **Parameters**: Use `const&` for read-only parameters
- **Null pointers**: Use `nullptr` instead of `NULL`
- **Type inference**: Use `auto` when type is obvious from context
- **Enums**: Use `enum class` for type safety

### Code Examples

**Class Definition:**
```cpp
class GpuBuffer
{
private:
    VkDevice m_device{ VK_NULL_HANDLE };
    VkDeviceSize m_size{ 0u };

public:
    GpuBuffer() = delete;
    GpuBuffer(const GpuBuffer& other) = delete;
    
    explicit GpuBuffer(VkDevice device);
    virtual ~GpuBuffer();
    
    bool create(const std::vector<float>& data);
    VkBuffer getBuffer() const;
};
```

**Enum Usage:**
```cpp
enum class ColorSpace {
    UNKNOWN,
    SRGB,
    BT709,
    BT2020
};

ColorSpace color_space = ColorSpace::SRGB;
if (color_space == ColorSpace::SRGB) { ... }
```

**Comments:**
- Write self-documenting code - minimize comments
- Comment "why", not "what"
- Use `//` for single-line comments
- Use blank `//` lines to separate logical sections

## Making Changes

### Workflow

1. **Create a branch** for your changes:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes** following the code style guide

3. **Test your changes**:
   ```bash
   cd build
   ninja
   # Run tests
   cd ..\bin
   PlaygroundSDK_test.exe
   ```

4. **Run code quality checks**:
   ```bash
   # Static analysis (from project root)
   cppcheck src --check-level=exhaustive --quiet --std=c++20 \
     --enable=style,performance,portability \
     --suppress=cstyleCast --suppress=useStlAlgorithm
   ```

5. **Commit your changes** with clear messages:
   ```bash
   git add .
   git commit -m "Add feature: brief description"
   ```

### Commit Messages

- Use present tense ("Add feature" not "Added feature")
- Start with a verb ("Fix", "Add", "Update", "Refactor")
- Keep first line under 72 characters
- Add detailed description after blank line if needed

**Examples:**
```
Add Texture3D class with cube sampling support

Implements 3D texture loading and sampling capabilities.
Includes unit tests and example usage in example03.
```

```
Fix buffer alignment issue in DescriptorBufferSet

Corrects offset calculation to respect device alignment requirements.
Resolves #42
```

## Project Structure

When adding new code, follow the existing organization:

```
src/
├── core/              # Foundation (math, I/O, utilities)
│   ├── color/
│   ├── image/
│   ├── io/
│   ├── math/
│   └── utility/
├── cpu/               # CPU-side implementations
├── gpu/               # GPU abstractions
│   ├── shader/        # Shader compilation (GLSL, SPIR-V, Slang)
│   └── vulkan/        # Vulkan-specific code
│       ├── builder/   # Complex object builders
│       ├── factory/   # Object creation factories
│       ├── utility/   # Helper functions
│       └── ...
└── engine/            # High-level engine code
    ├── renderer/      # Rendering abstractions
    └── runtime/       # Application framework
```

## Adding New Features

### Adding a New Class

1. Place headers and implementations together in appropriate `src/` subdirectory
2. Use existing classes as templates (e.g., `GpuBuffer.h`)
3. Add to the relevant umbrella header (e.g., `core/core.h`, `engine/engine.h`)
4. Write unit tests in `test/` directory

### Adding a New Example

1. Create a new directory: `example05/`
2. Add `main.cpp` and any supporting files
3. Update `CMakeLists.txt`:
   ```cmake
   collect_sources_and_headers(${CMAKE_SOURCE_DIR}/example05 SOURCES_EXAMPLE05)
   add_executable(Example05 ${SOURCES_EXAMPLE05})
   target_link_libraries(Example05 PRIVATE PlaygroundSDK volk::volk_headers unofficial::spirv-reflect)
   ```

### Adding Tests

- Place tests in `test/` directory
- Use Google Test framework
- Name test files descriptively (e.g., `core_math.cpp`, `gpu_vulkan.cpp`)
- Tests are automatically discovered by CMake

## Code Quality

### Static Analysis

The project uses:
- **clang-tidy**: Runs automatically during build
- **cppcheck**: Run manually before committing

Configuration files:
- `.clang-tidy` - clang-tidy rules

### Building with Checks

```bash
# Configure with clang-tidy enabled (default)
cmake --preset=ninja

# Build (clang-tidy runs automatically)
cd build
ninja
```

## Documentation

- Update relevant documentation in `docs/` when adding features
- Add inline comments for complex algorithms
- Keep comments concise - code should be self-documenting
- Update README.md if project structure changes

## Questions?

- Review existing code for examples
- Check documentation in `docs/`
- Open an issue for discussion before major changes

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (see [LICENSE](LICENSE)).

Thank you for contributing to Playground! 🚀
