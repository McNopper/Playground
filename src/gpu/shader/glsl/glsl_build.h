#ifndef GPU_SHADER_GLSL_SLANGBUILD_H_
#define GPU_SHADER_GLSL_SLANGBUILD_H_

#include <map>
#include <optional>
#include <string>

#include "gpu/shader/spirv/spirv_data.h"

std::optional<SpirvData> buildGlsl(const std::string& source, const std::map<std::string, std::string>& macros, SpvExecutionModel execution_model);

#endif /* GPU_SHADER_GLSL_SLANGBUILD_H_ */
