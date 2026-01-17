#include "filesystem.h"

#include <algorithm>
#include <filesystem>

std::string extension(const std::string& filename)
{
	std::filesystem::path filesystem_path{ filename };
	std::string extension = filesystem_path.extension().generic_string();
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	return extension;
}

std::string stem(const std::string& filename)
{
	std::filesystem::path filesystem_path{ filename };

	return filesystem_path.stem().generic_string();
}

std::string parentPath(const std::string& filename)
{
	std::filesystem::path filesystem_path{ filename };

	return filesystem_path.parent_path().generic_string();
}
