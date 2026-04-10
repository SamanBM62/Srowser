#pragma once

#include <cstddef>
#include <string>
#include <functional>
#include "DOM.hpp"

struct Parser {
    dom::Node parse(std::string);
    Parser(std::size_t pos, std::string input);
    
    private:
    
    std::size_t pos;
    std::string input;

    char next_char() const;
    bool starts_with(std::string const& s) const;
    void expect(std::string const& s);
    char consume_char();
    bool eof();
    std::string consume_while(std::function<bool(char)> test);
    void consume_whitespace();
    std::string parse_name();
    dom::Node text(std::string data);
    dom::Node parse_text();
    dom::Node parse_element();

    dom::Node parse_node();

    std::pair<std::string, std::string> parse_attr();
    std::string parse_attr_value();

    dom::ElementData::AttrMap parse_attributes();

    std::vector<dom::Node> parse_nodes();
};