#include "spirv_convert.h"

#include <spirv-tools/libspirv.hpp>

#include "core/utility/strings.h"

static std::vector<uint32_t> convertFromTo(const std::vector<uint32_t>& code, const std::string& from, const std::string& to)
{
    spvtools::SpirvTools spirv_tools(SPV_ENV_VULKAN_1_4);

    std::string disassembled{};
    if (!spirv_tools.Disassemble(code, &disassembled))
    {
        return {};
    }

	disassembled = replaceAll(disassembled, from, to);

    std::vector<uint32_t> assembled{};
    if (!spirv_tools.Assemble(disassembled, &assembled))
    {
        return {};
    }

    return assembled;
}

std::vector<uint32_t> convertToRowMajor(const std::vector<uint32_t>& code)
{
    return convertFromTo(code, " ColMajor", " RowMajor");
}

std::vector<uint32_t> convertToColumnMajor(const std::vector<uint32_t>& code)
{
    return convertFromTo(code, " RowMajor", " ColMajor");
}
