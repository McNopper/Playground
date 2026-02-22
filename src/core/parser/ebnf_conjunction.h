#ifndef CORE_PARSER_EBNF_CONJUNCTION_H_
#define CORE_PARSER_EBNF_CONJUNCTION_H_

#include <algorithm>
#include <ranges>
#include <string_view>

#include "ebnf_base.h"

namespace ebnf {

// Sequence operations

class Alternation : public ASequenceSymbol
{

public:

    Alternation() = default;

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (m_symbols.empty()) [[unlikely]]
        {
            return {};
        }

        // C++20: Use ranges to find first successful parse
        for (const auto& element : m_symbols)
        {
            auto result = element->parse(text, position);

            if (result.success)
            {
                executeOnSuccess(result.value);
                return result;
            }
        }

        return {};
    }

};

class Concatenation : public ASequenceSymbol
{

public:

    Concatenation() = default;

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (m_symbols.empty()) [[unlikely]]
        {
            return {};
        }

        std::string accumulated_value;
        std::size_t current_position = position;

        // C++20: All symbols must succeed in sequence
        for (const auto& element : m_symbols)
        {
            auto result = element->parse(text, current_position);

            if (!result.success)
            {
                return {};
            }

            accumulated_value += result.value;
            current_position = result.next_position;
        }

        executeOnSuccess(accumulated_value);

        return {.success = true, 
                .next_position = current_position, 
                .value = std::move(accumulated_value)};
    }

};

} // namespace ebnf

#endif /* CORE_PARSER_EBNF_CONJUNCTION_H_ */
