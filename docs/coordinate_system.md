# Coordinate System

This document defines the coordinate system conventions used throughout the Playground SDK.

## Conventions

### 3D Space

- **Coordinate System**: Right-handed coordinate system
- **Axis Orientation**: Y-axis points up
- **Front Face**: Counter-clockwise winding order
- **Matrix Layout**: Column-major storage

### 2D Texture/Image Space (UV Coordinates)

- **Image Files**: Top-left origin (standard for PNG, JPEG, etc.)
- **Memory Storage**: Top-left origin (matches file format)
- **GPU Textures**: Top-left origin (first row = top of image)
- **UV Coordinates**: Top-left origin (0,0)
  - Top vertices: V = 0.0 (samples first row = top)
  - Bottom vertices: V = 1.0 (samples last row = bottom)
- **Standard**: glTF/Vulkan UV convention (top-left)
- **Result**: Everything consistent - images display correctly

### Framebuffer/Viewport

- **Origin**: Top-left corner
- **Y Axis**: Increases downward
- **Standard**: Vulkan/DirectX convention (differs from OpenGL which uses bottom-left)

### Clip Space / NDC (Normalized Device Coordinates)

- **X Axis**: -1.0 (left) to +1.0 (right)
- **Y Axis**: -1.0 (top) to +1.0 (bottom) - **Y-down in Vulkan**
- **Z Axis**: 0.0 (near) to 1.0 (far) - Vulkan depth range
- **Important**: Vulkan clip space has Y pointing down, unlike OpenGL
- **Projection Matrix**: Must use negative Y-scale to convert from Y-up world space to Y-down clip space

### Rationale

These conventions ensure compatibility with:

- **Vulkan**: Y-down NDC (-1 = top, +1 = bottom), requires Y-flip in projection matrix
- **glTF Specification**: Right-handed Y-up world space, top-left UV origin
- **Image Formats**: PNG, JPEG, and most image formats use top-left origin
- **3D Modeling Tools**: Consistent with Blender and Maya conventions

## Usage

When implementing rendering code, ensure all transformations and vertex data follow these conventions:

- **World Space Positions**: Y-up (positive Y is up)
- **Clip Space Positions**: Y-down (negative Y is top, positive Y is bottom)
- **Texture Coordinates**: Use top-left origin UVs (top V=0.0, bottom V=1.0)
- **Projection Matrices**: Use negative Y-scale to flip from Y-up world to Y-down NDC
- **Image Loading**: Images loaded as-is (top-left origin) - no flipping needed
- **GPU Textures**: V=0 samples first row (top of image) - matches file format
- **Viewports**: Set viewport.y = 0.0f for top of screen, positive height

### Example: Vertex Data

```cpp
// For world space (with projection matrix):
float3 position_world = { 0.0f, +1.0f, 0.0f };  // Y=+1 is up
float2 uv = { 0.5f, 0.0f };                     // V=0 is top of texture

// For clip space (identity matrices, like example04):
float3 position_clip = { 0.0f, -0.5f, 0.0f };   // Y=-0.5 is top of screen
float2 uv = { 0.5f, 0.0f };                     // V=0 is top of texture
```
