#ifndef GPU_SHADER_SPIRV_SPIRVDATA_H_
#define GPU_SHADER_SPIRV_SPIRVDATA_H_

#include <cstdint>
#include <vector>

#include <spirv-reflect/spirv.h>

struct SpirvData
{
    std::vector<uint32_t> code{};
    SpvExecutionModel execution_model{};
};

#endif /* GPU_SHADER_SPIRV_SPIRVDATA_H_ */
