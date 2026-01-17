#ifndef CORE_IO_BINARYDATA_H_
#define CORE_IO_BINARYDATA_H_

#include <optional>
#include <string>

bool save(const std::string& output, const std::string& filename);

std::optional<std::string> load(const std::string& filename);

#endif /* CORE_IO_BINARYDATA_H_ */
