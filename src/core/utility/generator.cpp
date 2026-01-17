#include "generator.h"

#include <algorithm>

std::vector<uint32_t> generateIndices(uint32_t size)
{
	std::vector<uint32_t> result(size);
	std::generate(result.begin(), result.end(), [index = 0u]() mutable { return index++; });

	return result;
}
