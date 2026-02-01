# Third-Party Licenses

This project uses the following open-source libraries. All licenses are permissive and compatible with the project's MIT License.

---

## Apache License 2.0

### OpenImageIO
- **Project**: https://github.com/AcademySoftwareFoundation/OpenImageIO
- **Description**: Image loading, saving, and manipulation library
- **Used for**: Image I/O operations

### shaderc
- **Project**: https://github.com/google/shaderc
- **Description**: GLSL to SPIR-V compiler
- **Used for**: Shader compilation

### Slang
- **Project**: https://github.com/shader-slang/slang
- **Description**: Shader compiler for multiple target languages
- **Used for**: Slang shader compilation

### SPIRV-Reflect
- **Project**: https://github.com/KhronosGroup/SPIRV-Reflect
- **Description**: SPIR-V reflection and analysis library
- **Used for**: Shader reflection and descriptor set layout generation

### SPIRV-Tools
- **Project**: https://github.com/KhronosGroup/SPIRV-Tools
- **Description**: SPIR-V optimization and validation tools
- **Used for**: Shader optimization and validation

---

## MIT License

### Dear ImGui
- **Project**: https://github.com/ocornut/imgui
- **Description**: Immediate mode GUI library
- **Used for**: User interface rendering

### nlohmann/json
- **Project**: https://github.com/nlohmann/json
- **Description**: JSON parsing and serialization library
- **Used for**: JSON data handling

### volk
- **Project**: https://github.com/zeux/volk
- **Description**: Vulkan meta-loader for dynamic function loading
- **Used for**: Vulkan API dynamic loading

---

## zlib License

### GLFW
- **Project**: https://github.com/glfw/glfw
- **Description**: Multi-platform windowing and input library
- **Used for**: Window creation and input handling

### zlib
- **Project**: https://github.com/madler/zlib
- **Description**: General-purpose compression library
- **Used for**: Gzip compression/decompression

---

## BSD 3-Clause License

### Google Test (gtest)
- **Project**: https://github.com/google/googletest
- **Description**: C++ testing framework
- **Used for**: Unit testing

---

## External Dependencies (Not Managed by vcpkg)

### Vulkan SDK
- **Project**: https://vulkan.lunarg.com/
- **Description**: Vulkan graphics and compute API
- **Used for**: GPU programming
- **Note**: Requires `VULKAN_SDK` environment variable; Headers licensed under Apache 2.0, SDK contains multiple components with various licenses (see https://vulkan.lunarg.com/license/)
