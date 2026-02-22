#include "debug.h"

#include <cstdint>

std::string toString(const float2& value)
{
    std::string result{};

    for (uint32_t i = 0u; i < 2u; i++)
    {
        result += std::to_string(value[i]);
        if (i + 1u < 2u)
        {
            result += ",";
        }
        result += "\n";
    }

    return result;
}

std::string toString(const float3& value)
{
    std::string result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        result += std::to_string(value[i]);
        if (i + 1u < 3u)
        {
            result += ",";
        }
        result += "\n";
    }

    return result;
}

std::string toString(const float4& value)
{
    std::string result{};

    for (uint32_t i = 0u; i < 4u; i++)
    {
        result += std::to_string(value[i]);
        if (i + 1u < 4u)
        {
            result += ", ";
        }
        result += "\n";
    }

    return result;
}

std::string toString(const float2x2& value)
{
    std::string result{};

    for (uint32_t row = 0u; row < 2u; row++)
    {
        for (uint32_t column = 0u; column < 2u; column++)
        {
            result += std::to_string(value[column][row]);
            if (column + 1u < 2u || row + 1u < 2u)
            {
                result += ", ";
            }
            if (row + 1u == 2u)
            {
                result += "\n";
            }
        }
    }

    return result;
}

std::string toString(const float3x3& value)
{
    std::string result{};

    for (uint32_t row = 0u; row < 3u; row++)
    {
        for (uint32_t column = 0u; column < 3u; column++)
        {
            result += std::to_string(value[column][row]);
            if (column + 1u < 3u || row + 1u < 3u)
            {
                result += ", ";
            }
            if (row + 1u == 3u)
            {
                result += "\n";
            }
        }
    }

    return result;
}

std::string toString(const float4x4& value)
{
    std::string result{};

    for (uint32_t row = 0u; row < 4u; row++)
    {
        for (uint32_t column = 0u; column < 4u; column++)
        {
            result += std::to_string(value[column][row]);
            if (column + 1u < 4u || row + 1u < 4u)
            {
                result += ", ";
            }
            if (row + 1u == 4u)
            {
                result += "\n";
            }
        }
    }

    return result;
}

std::string toString(const quaternion& value)
{
    std::string result{};

    for (uint32_t i = 0u; i < 4u; i++)
    {
        result += std::to_string(value[i]);
        if (i + 1u < 4u)
        {
            result += ", ";
        }
        result += "\n";
    }

    return result;
}
