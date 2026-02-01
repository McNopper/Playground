#ifndef ENGINE_RENDERER_MATERIAL_MATERIALSHADER_H_
#define ENGINE_RENDERER_MATERIAL_MATERIALSHADER_H_

// Base class for material shaders
// Can be instantiated directly or derived from for specific shader types
// Inspired by OpenUSD's UsdShadeShader and MaterialX node conventions
class MaterialShader
{
public:

    MaterialShader() = default;
    virtual ~MaterialShader() = default;

    MaterialShader(const MaterialShader& other) = delete;
    MaterialShader& operator=(const MaterialShader& other) = delete;

};

#endif /* ENGINE_RENDERER_MATERIAL_MATERIALSHADER_H_ */
