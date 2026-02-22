#include "spirv_io.h"

#include <algorithm>

#include <spirv-reflect/spirv_reflect.h>

#include "core/io/binary_data.h"
#include "core/utility/convert.h"

#include "spirv_convert.h"

std::optional<SpirvData> loadSpirv(const std::string& filename)
{
    auto input = load(filename);
    if (!input.has_value())
    {
        return {};
    }

    SpirvData spirv_data{};
    spirv_data.code = stringToVector<uint32_t>(*input);
    if (spirv_data.code.empty())
    {
        return {};
    }

    //

    spirv_data.code = convertToColumnMajor(spirv_data.code);
    if (spirv_data.code.empty())
    {
        return {};
    }

    //

    SpvReflectShaderModule module{};
    SpvReflectResult result = spvReflectCreateShaderModule(spirv_data.code.size() * sizeof(uint32_t), spirv_data.code.data(), &module);
    if (result != SPV_REFLECT_RESULT_SUCCESS)
    {
        return {};
    }
    
    spirv_data.execution_model = module.spirv_execution_model;

    spvReflectDestroyShaderModule(&module);

    // Cross check, if we support internally.

    // Current execution models, which can be extended.
    std::vector<SpvExecutionModel> execution_model_checks {
        SpvExecutionModelVertex,
        SpvExecutionModelTessellationControl,
        SpvExecutionModelTessellationEvaluation,
        SpvExecutionModelGeometry,
        SpvExecutionModelFragment,
        SpvExecutionModelGLCompute
    };

    auto check = std::find(execution_model_checks.begin(), execution_model_checks.end(), spirv_data.execution_model);
    if (check == execution_model_checks.end())
    {
        return {};
    }

    return spirv_data;
}

bool saveSpirv(const std::string& filename, const SpirvData& spirv_data)
{
    auto output = vectorToString<uint32_t>(spirv_data.code);
    if (output.empty())
    {
        return false;
    }

    return save(output, filename);
}
