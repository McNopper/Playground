#include "binary_data.h"

#include <fstream>

bool save(const std::string& output, const std::string& filename)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	file.write(output.data(), output.size());
	file.close();

	return true;
}

std::optional<std::string> load(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		return {};
	}

	std::size_t file_size = static_cast<std::size_t>(file.tellg());
	file.seekg(0);

	std::string input{};
	input.resize(file_size);

	file.read(input.data(), file_size);
	file.close();

	return input;
}
