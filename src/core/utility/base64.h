
#ifndef CORE_UTILITY_BASE64_H_
#define CORE_UTILITY_BASE64_H_

#include <cstdint>
#include <string>
#include <vector>

std::string base64Encode(const std::uint8_t* data, std::size_t length);

std::string base64Encode(const std::vector<std::uint8_t>& data);

std::vector<std::uint8_t> base64Decode(const std::string& encoded);

#endif /* CORE_UTILITY_BASE64_H_ */
