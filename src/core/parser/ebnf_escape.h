#ifndef CORE_PARSER_EBNF_ESCAPE_H_
#define CORE_PARSER_EBNF_ESCAPE_H_

namespace ebnf {

// Simple escape sequences
// see https://en.cppreference.com/w/cpp/language/escape

constexpr char ESC_AUDIBLE_BELL = '\a';
constexpr char ESC_BACKSLASH = '\\';
constexpr char ESC_BACKSPACE = '\b';
constexpr char ESC_CARRIAGE_RETURN = '\r';
constexpr char ESC_DOUBLE_QUOTE = '\"';
constexpr char ESC_FORM_FEED = '\f';
constexpr char ESC_HORIZONTAL_TAB = '\t';
constexpr char ESC_LINE_FEED = '\n';
constexpr char ESC_SINGLE_QUOTE = '\'';
constexpr char ESC_QUESTION_MARK = '\?';
constexpr char ESC_VERTICAL_TAB = '\v';

} // namespace ebnf

#endif /* CORE_PARSER_EBNF_ESCAPE_H_ */
