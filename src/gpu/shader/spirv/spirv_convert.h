#ifndef GPU_SHADER_SPIRV_SPIRVCONVERT_H_
#define GPU_SHADER_SPIRV_SPIRVCONVERT_H_

#include <cstdint>
#include <vector>

#include "spirv_data.h"

std::vector<uint32_t> convertToRowMajor(const std::vector<uint32_t>& code);

std::vector<uint32_t> convertToColumnMajor(const std::vector<uint32_t>& code);

#endif /* GPU_SHADER_SPIRV_SPIRVCONVERT_H_ */
