#ifndef CORE_PARSER_EBNF_BASE_H_
#define CORE_PARSER_EBNF_BASE_H_

#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace ebnf {

// Parse result structure

struct ParseResult {
    bool success{false};
    std::size_t next_position{0};
    std::string value{};
    
    // C++20: Spaceship operator for comparisons
    auto operator<=>(const ParseResult&) const = default;
};

// Base symbol

class ASymbol {

private:

    std::function<void(std::string_view)> m_on_success{};

protected:

    void executeOnSuccess(std::string_view sequence) const noexcept
    {
        if (m_on_success)
        {
            m_on_success(sequence);
        }
    }

public:

    ASymbol() = default;

    virtual ~ASymbol() = default;
    
    // C++20: Prevent copying, allow moving
    ASymbol(const ASymbol&) = delete;
    ASymbol& operator=(const ASymbol&) = delete;
    ASymbol(ASymbol&&) = default;
    ASymbol& operator=(ASymbol&&) = default;

    virtual ParseResult parse(std::string_view text, std::size_t position) const = 0;

    void setOnSuccess(std::function<void(std::string_view)> on_success) noexcept
    {
        m_on_success = std::move(on_success);
    }

};

class ASingleSymbol : public ASymbol {

protected:

    std::shared_ptr<ASymbol> m_symbol;

public:

    ASingleSymbol() = delete;

    explicit ASingleSymbol(std::shared_ptr<ASymbol> symbol) :
        m_symbol{std::move(symbol)}
    {
    }

};

class ASequenceSymbol : public ASymbol {

protected:

    std::vector<std::shared_ptr<ASymbol>> m_symbols;

public:

    ASequenceSymbol() = default;

    virtual void append(std::shared_ptr<ASymbol> symbol)
    {
        m_symbols.push_back(std::move(symbol));
    }
    
    // C++20: Add range-based initialization
    template<std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, std::shared_ptr<ASymbol>>
    void append_range(R&& symbols)
    {
        std::ranges::copy(std::forward<R>(symbols), std::back_inserter(m_symbols));
    }

};

} // namespace ebnf

#endif /* CORE_PARSER_EBNF_BASE_H_ */
