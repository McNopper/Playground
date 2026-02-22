#ifndef CORE_PARSER_EBNF_TERMINAL_H_
#define CORE_PARSER_EBNF_TERMINAL_H_

#include <cstddef>
#include <string>
#include <string_view>

#include "ebnf_base.h"

namespace ebnf {

// Terminal

class Terminal : public ASymbol
{

protected:

    std::string m_character_sequence;

public:

    Terminal() = delete;

    explicit Terminal(char character) :
        m_character_sequence{character}
    {
    }

    explicit Terminal(std::string character_sequence) :
        m_character_sequence{std::move(character_sequence)}
    {
    }

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (m_character_sequence.empty()) [[unlikely]]
        {
            return {};
        }

        if (position + m_character_sequence.size() > text.size()) [[unlikely]]
        {
            return {};
        }

        // C++20: Use string_view for comparison
        if (text.substr(position, m_character_sequence.size()) != m_character_sequence)
        {
            return {};
        }

        executeOnSuccess(m_character_sequence);

        return {.success = true, 
                .next_position = position + m_character_sequence.size(), 
                .value = m_character_sequence};
    }

};

} // namespace ebnf

#endif /* CORE_PARSER_EBNF_TERMINAL_H_ */
