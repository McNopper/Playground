#ifndef CORE_PARSER_EBNF_FACTOR_H_
#define CORE_PARSER_EBNF_FACTOR_H_

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

#include "ebnf_base.h"

namespace ebnf {

// Factors

class ZeroOneFactor : public ASingleSymbol {

public:

    ZeroOneFactor() = delete;

    explicit ZeroOneFactor(std::shared_ptr<ASymbol> symbol) :
        ASingleSymbol(std::move(symbol))
    {
    }

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (!m_symbol) [[unlikely]]
        {
            return {};
        }

        auto result = m_symbol->parse(text, position);

        if (result.success)
        {
            executeOnSuccess(result.value);
            return result;
        }

        // C++20: Designated initializers
        executeOnSuccess("");
        return {.success = true, .next_position = position, .value = ""};
    }

};

class ZeroManyFactor : public ASingleSymbol {

public:

    ZeroManyFactor() = delete;

    explicit ZeroManyFactor(std::shared_ptr<ASymbol> symbol) :
        ASingleSymbol(std::move(symbol))
    {
    }

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (!m_symbol) [[unlikely]]
        {
            return {};
        }

        std::string accumulated_value;
        std::size_t current_position = position;

        // Match while possible
        for (auto result = m_symbol->parse(text, current_position); 
             result.success; 
             result = m_symbol->parse(text, current_position))
        {
            accumulated_value += result.value;
            current_position = result.next_position;
        }

        executeOnSuccess(accumulated_value);

        return {.success = true, 
                .next_position = current_position, 
                .value = std::move(accumulated_value)};
    }

};

class OneManyFactor : public ASingleSymbol {

public:

    OneManyFactor() = delete;

    explicit OneManyFactor(std::shared_ptr<ASymbol> symbol) :
        ASingleSymbol(std::move(symbol))
    {
    }

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (!m_symbol) [[unlikely]]
        {
            return {};
        }

        auto first_result = m_symbol->parse(text, position);

        if (!first_result.success)
        {
            return {};
        }

        std::string accumulated_value = std::move(first_result.value);
        std::size_t current_position = first_result.next_position;

        // Continue matching while possible
        for (auto result = m_symbol->parse(text, current_position); 
             result.success; 
             result = m_symbol->parse(text, current_position))
        {
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

#endif /* CORE_PARSER_EBNF_FACTOR_H_ */
