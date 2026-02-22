#ifndef CORE_PARSER_EBNF_EXCLUDE_H_
#define CORE_PARSER_EBNF_EXCLUDE_H_

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

#include "ebnf_base.h"

namespace ebnf {

// Exclude (aka. exception)

class Exclude : public ASingleSymbol
{

protected:

    std::shared_ptr<ASymbol> m_exclude_symbol;

public:

    Exclude() = delete;

    Exclude(std::shared_ptr<ASymbol> symbol, std::shared_ptr<ASymbol> exclude_symbol) :
        ASingleSymbol(std::move(symbol)), m_exclude_symbol{std::move(exclude_symbol)}
    {
    }

    ParseResult parse(std::string_view text, std::size_t position) const override
    {
        if (!m_symbol || !m_exclude_symbol) [[unlikely]]
        {
            return {};
        }

        auto main_result = m_symbol->parse(text, position);

        if (!main_result.success)
        {
            return {};
        }

        // C++20: Check if exclude pattern also matches at the same position
        auto exclude_result = m_exclude_symbol->parse(text, position);

        if (exclude_result.success)
        {
            // Both A and B match at the same position - this is excluded
            return {};
        }

        executeOnSuccess(main_result.value);

        return main_result;
    }
};

} // namespace ebnf

#endif /* CORE_PARSER_EBNF_EXCLUDE_H_ */
