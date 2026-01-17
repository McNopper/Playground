#ifndef CORE_IO_FILESYSTEM_H_
#define CORE_IO_FILESYSTEM_H_

#include <string>

std::string extension(const std::string& filename);

std::string stem(const std::string& filename);

std::string parentPath(const std::string& filename);

#endif /* CORE_IO_FILESYSTEM_H_ */
