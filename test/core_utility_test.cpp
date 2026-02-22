#include <cstdint>
#include <string>
#include <vector>

#include <gtest/gtest.h>

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
        { (const std::uint8_t*)a.data(),
          a.size() * sizeof(std::uint32_t),
          sizeof(std::uint32_t) });
    raw_data.push_back(
        { (const std::uint8_t*)b.data(),
          b.size() * sizeof(double),
          sizeof(double) });

    std::size_t stride{ 0u };
    std::vector<std::uint8_t> interleaved = interleaveData(raw_data, stride);

    EXPECT_EQ(stride, sizeof(std::uint32_t) + sizeof(double));
    EXPECT_EQ(interleaved.size(), 3u * stride);

    EXPECT_EQ(*(const std::uint32_t*)(interleaved.data() + (1u * stride)), 2u);
}

TEST(TestUtility, Base64EncodeEmpty)
{
    std::vector<std::uint8_t> empty{};
    std::string encoded = base64Encode(empty);
    EXPECT_EQ(encoded, "");
}

TEST(TestUtility, Base64DecodeEmpty)
{
    std::vector<std::uint8_t> decoded = base64Decode("");
    EXPECT_TRUE(decoded.empty());
}

TEST(TestUtility, Base64EncodeSimple)
{
    std::vector<std::uint8_t> data{ 'M', 'a', 'n' };
    std::string encoded = base64Encode(data);
    EXPECT_EQ(encoded, "TWFu");
}

TEST(TestUtility, Base64DecodeSimple)
{
    std::vector<std::uint8_t> decoded = base64Decode("TWFu");
    std::vector<std::uint8_t> expected{ 'M', 'a', 'n' };
    EXPECT_EQ(decoded, expected);
}

TEST(TestUtility, Base64EncodePadding1)
{
    std::vector<std::uint8_t> data{ 'M', 'a' };
    std::string encoded = base64Encode(data);
    EXPECT_EQ(encoded, "TWE=");
}

TEST(TestUtility, Base64DecodePadding1)
{
    std::vector<std::uint8_t> decoded = base64Decode("TWE=");
    std::vector<std::uint8_t> expected{ 'M', 'a' };
    EXPECT_EQ(decoded, expected);
}

TEST(TestUtility, Base64EncodePadding2)
{
    std::vector<std::uint8_t> data{ 'M' };
    std::string encoded = base64Encode(data);
    EXPECT_EQ(encoded, "TQ==");
}

TEST(TestUtility, Base64DecodePadding2)
{
    std::vector<std::uint8_t> decoded = base64Decode("TQ==");
    std::vector<std::uint8_t> expected{ 'M' };
    EXPECT_EQ(decoded, expected);
}

TEST(TestUtility, Base64EncodeAllCharacters)
{
    std::vector<std::uint8_t> data{};
    data.reserve(64);
    for (std::uint8_t i = 0; i < 64; ++i)
    {
        data.push_back(i);
    }

    std::string encoded = base64Encode(data);
    std::vector<std::uint8_t> decoded = base64Decode(encoded);

    EXPECT_EQ(decoded, data);
}

TEST(TestUtility, Base64RoundTripBinary)
{
    std::vector<std::uint8_t> data{ 0x00, 0xFF, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
    std::string encoded = base64Encode(data);
    std::vector<std::uint8_t> decoded = base64Decode(encoded);
    EXPECT_EQ(decoded, data);
}

TEST(TestUtility, Base64RoundTripString)
{
    std::string text = "Hello, World! This is a base64 encoding test.";
    std::vector<std::uint8_t> data{ text.begin(), text.end() };

    std::string encoded = base64Encode(data);
    std::vector<std::uint8_t> decoded = base64Decode(encoded);

    std::string result{ decoded.begin(), decoded.end() };
    EXPECT_EQ(result, text);
}

TEST(TestUtility, Base64DecodeInvalidLength)
{
    std::vector<std::uint8_t> decoded = base64Decode("ABC");
    EXPECT_TRUE(decoded.empty());
}

TEST(TestUtility, Base64DecodeInvalidCharacter)
{
    std::vector<std::uint8_t> decoded = base64Decode("TW@u");
    EXPECT_TRUE(decoded.empty());
}

TEST(TestUtility, Base64EncodePointer)
{
    const std::uint8_t data[] = { 'T', 'e', 's', 't' };
    std::string encoded = base64Encode(data, 4);
    EXPECT_EQ(encoded, "VGVzdA==");
}

TEST(TestUtility, Base64EncodeNullPointer)
{
    std::string encoded = base64Encode(nullptr, 10);
    EXPECT_EQ(encoded, "");
}

TEST(TestUtility, GzipCompressEmpty)
{
    std::vector<std::uint8_t> empty{};
    std::vector<std::uint8_t> compressed = gzipCompress(empty);
    EXPECT_TRUE(compressed.empty());
}

TEST(TestUtility, GzipDecompressEmpty)
{
    std::vector<std::uint8_t> decompressed = gzipDecompress(std::vector<std::uint8_t>{});
    EXPECT_TRUE(decompressed.empty());
}

TEST(TestUtility, GzipCompressSimple)
{
    std::string text = "Hello, World!";
    std::vector<std::uint8_t> data{ text.begin(), text.end() };

    std::vector<std::uint8_t> compressed = gzipCompress(data);
    EXPECT_FALSE(compressed.empty());
    EXPECT_LT(compressed.size(), 100u);
}

TEST(TestUtility, GzipRoundTripSimple)
{
    std::string text = "Hello, World!";
    std::vector<std::uint8_t> original{ text.begin(), text.end() };

    std::vector<std::uint8_t> compressed = gzipCompress(original);
    std::vector<std::uint8_t> decompressed = gzipDecompress(compressed);

    EXPECT_EQ(decompressed, original);
}

TEST(TestUtility, GzipRoundTripLarge)
{
    std::vector<std::uint8_t> original{};
    original.reserve(10000);
    for (std::uint32_t i = 0; i < 10000; ++i)
    {
        original.push_back(static_cast<std::uint8_t>(i % 256));
    }

    std::vector<std::uint8_t> compressed = gzipCompress(original);
    std::vector<std::uint8_t> decompressed = gzipDecompress(compressed);

    EXPECT_EQ(decompressed.size(), original.size());
    EXPECT_EQ(decompressed, original);
}

TEST(TestUtility, GzipRoundTripBinary)
{
    std::vector<std::uint8_t> original{ 0x00, 0xFF, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };

    std::vector<std::uint8_t> compressed = gzipCompress(original);
    std::vector<std::uint8_t> decompressed = gzipDecompress(compressed);

    EXPECT_EQ(decompressed, original);
}

TEST(TestUtility, GzipCompressionLevels)
{
    std::string text = "The quick brown fox jumps over the lazy dog. ";
    std::string repeated{};
    for (int i = 0; i < 100; ++i)
    {
        repeated += text;
    }
    std::vector<std::uint8_t> data{ repeated.begin(), repeated.end() };

    std::vector<std::uint8_t> fast = gzipCompress(data, 1);
    std::vector<std::uint8_t> best = gzipCompress(data, 9);

    EXPECT_FALSE(fast.empty());
    EXPECT_FALSE(best.empty());
    EXPECT_LE(best.size(), fast.size());

    std::vector<std::uint8_t> decompressed_fast = gzipDecompress(fast);
    std::vector<std::uint8_t> decompressed_best = gzipDecompress(best);

    EXPECT_EQ(decompressed_fast, data);
    EXPECT_EQ(decompressed_best, data);
}

TEST(TestUtility, GzipCompressPointer)
{
    const std::uint8_t data[] = { 'T', 'e', 's', 't' };
    std::vector<std::uint8_t> compressed = gzipCompress(data, 4);
    EXPECT_FALSE(compressed.empty());

    std::vector<std::uint8_t> decompressed = gzipDecompress(compressed);
    std::vector<std::uint8_t> expected{ 'T', 'e', 's', 't' };
    EXPECT_EQ(decompressed, expected);
}

TEST(TestUtility, GzipCompressNullPointer)
{
    std::vector<std::uint8_t> compressed = gzipCompress(nullptr, 10);
    EXPECT_TRUE(compressed.empty());
}

TEST(TestUtility, GzipDecompressInvalid)
{
    std::vector<std::uint8_t> invalid{ 0x00, 0x01, 0x02, 0x03 };
    std::vector<std::uint8_t> decompressed = gzipDecompress(invalid);
    EXPECT_TRUE(decompressed.empty());
}

TEST(TestUtility, GzipCompressHighlyCompressible)
{
    std::vector<std::uint8_t> repetitive{};
    repetitive.reserve(1000);
    for (int i = 0; i < 1000; ++i)
    {
        repetitive.push_back('A');
    }

    std::vector<std::uint8_t> compressed = gzipCompress(repetitive);

    EXPECT_LT(compressed.size(), repetitive.size() / 10);

    std::vector<std::uint8_t> decompressed = gzipDecompress(compressed);
    EXPECT_EQ(decompressed, repetitive);
}
