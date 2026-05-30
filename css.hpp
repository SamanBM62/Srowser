#pragma once

#include <optional>
#include <variant>
#include <string>
#include <vector>
#include <cstdint>
#include "parser.hpp"

class CSS: private Parser{
    struct SimpleSelector {
        std::optional<std::string> tag_name;
        std::optional<std::string> id;
        std::vector<std::string> _class;
    };
    struct Px
    {
        std::uint8_t value;
    };

    struct Color
    {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    };
    using Selector = std::variant<SimpleSelector>;
    using Value = std::variant<std::string /* keyword */ , float, std::uint8_t, /* length */ CSS::Color /* color value */>;
    
    struct Declaration {
        std::string name;
        Value value;
    };

    struct Rule
    {
        std::vector<Selector> selectors;
    };
    
    
    std::variant <Px> Unit;
    using Specificity = std::tuple<std::size_t, std::size_t, std::size_t>;
    


    CSS::SimpleSelector parse_simple_seceltor();
    static bool valid_identifier_char(char c);
    std::string parse_identifier();

    Specificity specificity(const Selector& selector);
    std::vector<Selector> parse_selectors();
    Rule parse_rule();
    Declaration parse_declaration();
    Value parse_value();
};