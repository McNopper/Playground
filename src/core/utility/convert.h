
#ifndef CORE_UTILITY_CONVERT_H_
#define CORE_UTILITY_CONVERT_H_

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

struct RawData {
    const uint8_t* data{ nullptr };
    std::size_t length{ 0u };
    std::size_t stride{ 0u };
};

template<class T>
std::vector<T> stringToVector(const std::string& s)
{
    if (s.size() % sizeof(T) != 0u)
    {
        return {};
    }

    std::vector<T> output{};
    output.resize(s.size() / sizeof(T));
    std::memcpy(output.data(), s.data(), s.size());

    return output;
}

template<class T>
std::string vectorToString(const std::vector<T>& v)
{
    std::string output{};
    output.resize(v.size() * sizeof(T));
    std::memcpy(output.data(), v.data(), v.size() * sizeof(T));

    return output;
}

std::vector<uint8_t> interleaveData(const std::vector<RawData>& raw_data, std::size_t& stride);

#endif /* CORE_UTILITY_CONVERT_H_ */
