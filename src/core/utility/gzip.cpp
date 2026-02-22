
#include "gzip.h"

#include <zlib.h>

namespace
{
constexpr std::size_t CHUNK_SIZE = 16384;
}

std::vector<std::uint8_t> gzipCompress(const std::uint8_t* data, std::size_t length, int level)
{
    if (data == nullptr || length == 0)
    {
        return {};
    }

    if (level < -1 || level > 9)
    {
        level = Z_DEFAULT_COMPRESSION;
    }

    z_stream stream{};
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_in = const_cast<Bytef*>(data);
    stream.avail_in = static_cast<uInt>(length);

    // windowBits = 15 + 16 for gzip format
    int result = deflateInit2(&stream, level, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
    if (result != Z_OK)
    {
        return {};
    }

    std::vector<std::uint8_t> compressed{};
    compressed.reserve(length / 2);

    std::uint8_t buffer[CHUNK_SIZE]{};

    do
    {
        stream.next_out = buffer;
        stream.avail_out = CHUNK_SIZE;

        result = deflate(&stream, Z_FINISH);
        if (result == Z_STREAM_ERROR)
        {
            deflateEnd(&stream);
            return {};
        }

        std::size_t have = CHUNK_SIZE - stream.avail_out;
        compressed.insert(compressed.end(), buffer, buffer + have);

    } while (stream.avail_out == 0);

    deflateEnd(&stream);

    if (result != Z_STREAM_END)
    {
        return {};
    }

    return compressed;
}

std::vector<std::uint8_t> gzipCompress(const std::vector<std::uint8_t>& data, int level)
{
    return gzipCompress(data.data(), data.size(), level);
}

std::vector<std::uint8_t> gzipDecompress(const std::uint8_t* data, std::size_t length)
{
    if (data == nullptr || length == 0)
    {
        return {};
    }

    z_stream stream{};
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_in = const_cast<Bytef*>(data);
    stream.avail_in = static_cast<uInt>(length);

    // windowBits = 15 + 16 for gzip format
    int result = inflateInit2(&stream, 15 + 16);
    if (result != Z_OK)
    {
        return {};
    }

    std::vector<std::uint8_t> decompressed{};
    decompressed.reserve(length * 2);

    std::uint8_t buffer[CHUNK_SIZE]{};

    do
    {
        stream.next_out = buffer;
        stream.avail_out = CHUNK_SIZE;

        result = inflate(&stream, Z_NO_FLUSH);

        if (result == Z_STREAM_ERROR || result == Z_DATA_ERROR || result == Z_MEM_ERROR)
        {
            inflateEnd(&stream);
            return {};
        }

        std::size_t have = CHUNK_SIZE - stream.avail_out;
        decompressed.insert(decompressed.end(), buffer, buffer + have);

    } while (stream.avail_out == 0);

    inflateEnd(&stream);

    if (result != Z_STREAM_END && result != Z_OK)
    {
        return {};
    }

    return decompressed;
}

std::vector<std::uint8_t> gzipDecompress(const std::vector<std::uint8_t>& data)
{
    return gzipDecompress(data.data(), data.size());
}
