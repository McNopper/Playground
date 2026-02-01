
#ifndef CORE_UTILITY_GZIP_H_
#define CORE_UTILITY_GZIP_H_

#include <cstdint>
#include <vector>

std::vector<std::uint8_t> gzipCompress(const std::uint8_t* data, std::size_t length, int level = -1);

std::vector<std::uint8_t> gzipCompress(const std::vector<std::uint8_t>& data, int level = -1);

std::vector<std::uint8_t> gzipDecompress(const std::uint8_t* data, std::size_t length);

std::vector<std::uint8_t> gzipDecompress(const std::vector<std::uint8_t>& data);

#endif /* CORE_UTILITY_GZIP_H_ */
