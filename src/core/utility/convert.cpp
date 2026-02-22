#include "convert.h"

std::vector<uint8_t> interleaveData(const std::vector<RawData>& raw_data, std::size_t& stride)
{
    std::vector<uint8_t> result{};

    std::size_t total_length{ 0u };
    std::size_t elements{ 0u };
    for (std::size_t i = 0u; i < raw_data.size(); i++)
    {
        if (i > 0u)
        {
            // All elements need to have the same amount of elements.
            if ((raw_data[i].length / raw_data[i].stride) != elements)
            {
                stride = 0u;

                return result;
            }
        }
        else
        {
            elements = raw_data[i].length / raw_data[i].stride;
        }

        total_length += raw_data[i].length;
        stride += raw_data[i].stride;
    }

    result.resize(total_length);

    std::size_t offset{ 0u };
    for (std::size_t i = 0u; i < elements; i++)
    {
        for (std::size_t j = 0u; j < raw_data.size(); j++)
        {
            memcpy(result.data() + offset, raw_data[j].data + (raw_data[j].stride * i), raw_data[j].stride);

            offset += raw_data[j].stride;
        }
    }

    return result;
}
