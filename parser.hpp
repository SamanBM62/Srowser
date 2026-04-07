#pragma once

#include <cstddef>
#include <string>
#include <functional>

struct Parser {
    std::size_t pos;
    std::string input;

    char next_char() const;
    bool starts_with(std::string& s) const;
    void expect(std::string& s);
    char consume_char();
    bool eof();
    std::string consume_while(std::function<bool(char)> test);
    void consume_whitespace();

    private:
    static bool is_whitespace(char c);
};