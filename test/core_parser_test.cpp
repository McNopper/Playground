#include <gtest/gtest.h>

#include "core/parser/parser.h"

// Test basic terminal parsing
TEST(EBNF_Parser, BasicTerminal)
{
    const std::string text{ "HelloWorld" };
    ebnf::Terminal hello{ "Hello" };

    auto result = hello.parse(text, 0);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.next_position, 5);
    EXPECT_EQ(result.value, "Hello");
}

// Test alternation (choice)
TEST(EBNF_Parser, Alternation)
{
    const std::string text{ "World" };

    auto hello = std::make_shared<ebnf::Terminal>("Hello");
    auto world = std::make_shared<ebnf::Terminal>("World");

    ebnf::Alternation choice{};
    choice.append(hello);
    choice.append(world);

    auto result = choice.parse(text, 0);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value, "World");
}

// Test repetition (zero or more)
TEST(EBNF_Parser, ZeroManyRepetition)
{
    const std::string text{ "aaabbb" };

    auto a_term = std::make_shared<ebnf::Terminal>("a");
    ebnf::ZeroManyFactor many_a{ a_term };

    auto result = many_a.parse(text, 0);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value, "aaa");
    EXPECT_EQ(result.next_position, 3);
}

// Test character range (e.g., digit)
TEST(EBNF_Parser, DigitRange)
{
    const std::string text{ "12345abc" };

    ebnf::IntervalCharacterRule digit{ '0', '9' };

    auto result = digit.parse(text, 0);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value, "12345");
    EXPECT_EQ(result.next_position, 5);
}

// Test concatenation
TEST(EBNF_Parser, Concatenation)
{
    const std::string text{ "abc123" };

    auto letters = std::make_shared<ebnf::IntervalCharacterRule>('a', 'z');
    auto digits = std::make_shared<ebnf::IntervalCharacterRule>('0', '9');

    ebnf::Concatenation sequence{};
    sequence.append(letters);
    sequence.append(digits);

    auto result = sequence.parse(text, 0);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value, "abc123");
}

// Test exception (exclude)
TEST(EBNF_Parser, Exception)
{
    const std::string text{ "5" };

    auto digit = std::make_shared<ebnf::IntervalCharacterRule>('0', '9');
    auto zero = std::make_shared<ebnf::Terminal>('0');

    ebnf::Exclude non_zero{ digit, zero };

    // Should match '5'
    auto result1 = non_zero.parse(text, 0);
    EXPECT_TRUE(result1.success);
    EXPECT_EQ(result1.value, "5");

    // Should NOT match '0'
    const std::string text2{ "0" };
    auto result2 = non_zero.parse(text2, 0);
    EXPECT_FALSE(result2.success);
}
