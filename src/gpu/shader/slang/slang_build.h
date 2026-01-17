#ifndef GPU_SHADER_SLANG_SLANGBUILD_H_
#define GPU_SHADER_SLANG_SLANGBUILD_H_

#include <map>
#include <string>
#include <vector>

#include "gpu/shader/spirv/spirv_data.h"

std::vector<SpirvData> buildSlang(const std::string& filename, const std::map<std::string, std::string>& macros, const std::string& include_path);

#endif /* GPU_SHADER_SLANG_SLANGBUILD_H_ */
