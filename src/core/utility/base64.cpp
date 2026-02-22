
#include "base64.h"

namespace
{
constexpr char ENCODING_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr char PADDING_CHAR = '=';

constexpr std::uint8_t INVALID_VALUE = 0xFF;

constexpr std::uint8_t getDecodingValue(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return static_cast<std::uint8_t>(c - 'A');
    }
    if (c >= 'a' && c <= 'z')
    {
        return static_cast<std::uint8_t>(c - 'a' + 26);
    }
    if (c >= '0' && c <= '9')
    {
        return static_cast<std::uint8_t>(c - '0' + 52);
    }
    if (c == '+')
    {
        return 62;
    }
    if (c == '/')
    {
        return 63;
    }
    return INVALID_VALUE;
}
} // namespace

std::string base64Encode(const std::uint8_t* data, std::size_t length)
{
    if (data == nullptr || length == 0)
    {
        return {};
    }

    std::size_t encoded_length = ((length + 2) / 3) * 4;
    std::string result{};
    result.reserve(encoded_length);

    std::size_t i{ 0 };
    for (; i + 2 < length; i += 3)
    {
        std::uint32_t triple = (static_cast<std::uint32_t>(data[i]) << 16) |
                               (static_cast<std::uint32_t>(data[i + 1]) << 8) |
                               static_cast<std::uint32_t>(data[i + 2]);

        result += ENCODING_TABLE[(triple >> 18) & 0x3F];
        result += ENCODING_TABLE[(triple >> 12) & 0x3F];
        result += ENCODING_TABLE[(triple >> 6) & 0x3F];
        result += ENCODING_TABLE[triple & 0x3F];
    }

    if (i < length)
    {
        std::uint32_t triple = static_cast<std::uint32_t>(data[i]) << 16;
        if (i + 1 < length)
        {
            triple |= static_cast<std::uint32_t>(data[i + 1]) << 8;
        }

        result += ENCODING_TABLE[(triple >> 18) & 0x3F];
        result += ENCODING_TABLE[(triple >> 12) & 0x3F];

        if (i + 1 < length)
        {
            result += ENCODING_TABLE[(triple >> 6) & 0x3F];
        }
        else
        {
            result += PADDING_CHAR;
        }

        result += PADDING_CHAR;
    }

    return result;
}

std::string base64Encode(const std::vector<std::uint8_t>& data)
{
    return base64Encode(data.data(), data.size());
}

std::vector<std::uint8_t> base64Decode(const std::string& encoded)
{
    if (encoded.empty())
    {
        return {};
    }

    std::size_t length = encoded.length();
    if (length % 4 != 0)
    {
        return {};
    }

    std::size_t padding{ 0 };
    if (encoded[length - 1] == PADDING_CHAR)
    {
        padding++;
        if (encoded[length - 2] == PADDING_CHAR)
        {
            padding++;
        }
    }

    std::size_t decoded_length = (length / 4) * 3 - padding;
    std::vector<std::uint8_t> result{};
    result.reserve(decoded_length);

    for (std::size_t i = 0; i < length; i += 4)
    {
        std::uint8_t a = getDecodingValue(encoded[i]);
        std::uint8_t b = getDecodingValue(encoded[i + 1]);
        std::uint8_t c = (i + 2 < length && encoded[i + 2] != PADDING_CHAR) ? getDecodingValue(encoded[i + 2]) : 0;
        std::uint8_t d = (i + 3 < length && encoded[i + 3] != PADDING_CHAR) ? getDecodingValue(encoded[i + 3]) : 0;

        if (a == INVALID_VALUE || b == INVALID_VALUE ||
            (encoded[i + 2] != PADDING_CHAR && c == INVALID_VALUE) ||
            (encoded[i + 3] != PADDING_CHAR && d == INVALID_VALUE))
        {
            return {};
        }

        std::uint32_t triple = (static_cast<std::uint32_t>(a) << 18) |
                               (static_cast<std::uint32_t>(b) << 12) |
                               (static_cast<std::uint32_t>(c) << 6) |
                               static_cast<std::uint32_t>(d);

        result.push_back(static_cast<std::uint8_t>((triple >> 16) & 0xFF));

        if (i + 2 < length && encoded[i + 2] != PADDING_CHAR)
        {
            result.push_back(static_cast<std::uint8_t>((triple >> 8) & 0xFF));
        }

        if (i + 3 < length && encoded[i + 3] != PADDING_CHAR)
        {
            result.push_back(static_cast<std::uint8_t>(triple & 0xFF));
        }
    }

    return result;
}
