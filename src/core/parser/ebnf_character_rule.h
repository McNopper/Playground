#ifndef CORE_PARSER_EBNF_CHARACTER_RULE_H_
#define CORE_PARSER_EBNF_CHARACTER_RULE_H_

#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_set>

#include "ebnf_base.h"

namespace ebnf {

// Character rules

class ACharacterRule : public ASymbol {

protected:

    std::unordered_set<char> m_characters_ignored;

public:

    ACharacterRule() = default;

    void addIgnoredCharacter(char ignore_character)
    {
        m_characters_ignored.insert(ignore_character);
    }

    [[nodiscard]] bool isIgnored(char c) const noexcept
    {
        return m_characters_ignored.contains(c);
    }

};

class IntervalCharacterRule : public ACharacterRule {

protected:

    char m_character_start{'\0'};
    char m_character_end{'\0'};

public:

    IntervalCharacterRule() = delete;

    IntervalCharacterRule(char character_start, char character_end) :
        m_character_start{character_start}, m_character_end{character_end}
    {
    }

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (m_character_start > m_character_end) [[unlikely]]
        {
            return {};
        }

        if (position >= text.size()) [[unlikely]]
        {
            return {};
        }

        std::string result;
        std::size_t current_pos = position;

        // C++20: Use ranges view with take_while
        for (char c : text | std::views::drop(position))
        {
            if (c < m_character_start || c > m_character_end)
                break;
            
            if (isIgnored(c))
                break;

            result += c;
            ++current_pos;
        }

        if (result.empty())
        {
            return {};
        }

        executeOnSuccess(result);

        return {.success = true, .next_position = current_pos, .value = std::move(result)};
    }

};

class AnyCharacterRule : public ACharacterRule {

public:

    AnyCharacterRule() = default;

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (position >= text.size()) [[unlikely]]
        {
            return {};
        }

        std::string result;
        std::size_t current_pos = position;

        // C++20: Use ranges with filter
        for (char c : text | std::views::drop(position))
        {
            if (isIgnored(c))
                break;

            result += c;
            ++current_pos;
        }

        if (result.empty())
        {
            return {};
        }

        executeOnSuccess(result);

        return {.success = true, .next_position = current_pos, .value = std::move(result)};
    }

};

} // namespace ebnf

#endif /* CORE_PARSER_EBNF_CHARACTER_RULE_H_ */
