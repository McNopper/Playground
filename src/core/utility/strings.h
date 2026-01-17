
#ifndef CORE_UTILITY_STRINGS_H_
#define CORE_UTILITY_STRINGS_H_

#include <string>

std::string beforeString(const std::string& source, const std::string& token);

std::string afterString(const std::string& source, const std::string& token);

std::string replaceAll(const std::string& source, const std::string& from, const std::string& to);

#endif /* CORE_UTILITY_STRINGS_H_ */
