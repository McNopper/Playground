#include "strings.h"

std::string beforeString(const std::string& source, const std::string& token)
{
    size_t position = source.find_first_of(token);

    if (position != std::string::npos)
	{
        return source.substr(0, position);
    }
	
	return source;
}

std::string afterString(const std::string& source, const std::string& token)
{
    size_t position = source.find_last_of(token);

    if (position != std::string::npos)
	{
        return source.substr(position + token.size());
    }
	
	return source;
}

std::string replaceAll(const std::string& source, const std::string& from, const std::string& to)
{
    std::string result{source};

    std::size_t start_pos = result.find(from);
    while (start_pos != std::string::npos)
    {
        result.replace(start_pos, from.length(), to);

        start_pos = result.find(from);
    }

    return result;
}
