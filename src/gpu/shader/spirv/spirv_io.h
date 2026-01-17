#ifndef GPU_SHADER_SPIRV_SPIRVIO_H_
#define GPU_SHADER_SPIRV_SPIRVIO_H_

#include <optional>
#include <string>

#include "spirv_data.h"

std::optional<SpirvData> loadSpirv(const std::string& filename);

bool saveSpirv(const std::string& filename, const SpirvData& spirv_data);

#endif /* GPU_SHADER_SPIRV_SPIRVIO_H_ */
