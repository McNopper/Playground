#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

#include "core/core.h"

TEST(TestUtility, Convert01)
{
    std::vector<std::uint8_t> original{ 'H', 'e', 'l', 'l', 'o' };
    std::string converted = vectorToString<std::uint8_t>(original);

    EXPECT_EQ(converted.size(), original.size());

    if (converted.size() == original.size())
    {
        for (std::size_t i = 0u; i < original.size(); i++)
        {
            EXPECT_EQ(converted[i], original[i]);
        }
    }
}

TEST(TestUtility, Convert02)
{
    std::vector<std::uint32_t> original{ 0u, 10u, 100u, 1000u, 10000u, 100000u };
    std::string temporary = vectorToString<std::uint32_t>(original);
    auto converted = stringToVector<std::uint32_t>(temporary);

    EXPECT_EQ(converted.size(), original.size());

    if (converted.size() == original.size())
    {
        for (std::size_t i = 0u; i < original.size(); i++)
        {
            EXPECT_EQ(converted[i], original[i]);
        }
    }
}

TEST(TestUtility, Interleave)
{
    std::vector<std::uint32_t> a{ 1u, 2u, 4u };
    std::vector<double> b{ 10.0, 100.0, 1000.0 };

    std::vector<RawData> raw_data{};
    raw_data.push_back(
        {
            (const std::uint8_t*)a.data(),
            a.size() * sizeof(std::uint32_t),
            sizeof(std::uint32_t) 
        }
    );
    raw_data.push_back(
        {
            (const std::uint8_t*)b.data(),
            b.size() * sizeof(double),
            sizeof(double)
        }
    );

    std::size_t stride{ 0u };
    std::vector<std::uint8_t> interleaved = interleaveData(raw_data, stride);

    EXPECT_EQ(stride, sizeof(std::uint32_t) + sizeof(double));
    EXPECT_EQ(interleaved.size(), 3u * stride);

    EXPECT_EQ(*(const std::uint32_t*)(interleaved.data() + (1u * stride)), 2u);
}
