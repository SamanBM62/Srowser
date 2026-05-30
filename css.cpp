#include "css.hpp"
#include <stdexcept>
#include <format>
#include <algorithm>

CSS::SimpleSelector CSS::parse_simple_seceltor() {
    auto selector{SimpleSelector{std::nullopt, std::nullopt, std::vector<std::string>{}}};
    while (!this->eof()) {
        auto check {this->next_char()};
        switch (check)
        {
        case '#':
            this->consume_char();
            selector.id = std::optional<std::string>{this->parse_identifier()};
            break;
        case '.':
            this->consume_char();
            selector._class.push_back(this->parse_identifier());
            break;
        case '*': 
            this->consume_char();
            break;
        default:
            break;
        }
        if (this->valid_identifier_char(check))
            selector.tag_name = std::optional<std::string>{this->parse_identifier()};
    }

    return selector;
}

bool CSS::valid_identifier_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) ||
           c == '-' ||
           c == '_';
}



std::string CSS::parse_identifier() {
    return this->consume_while(CSS::valid_identifier_char);
}

CSS::Specificity CSS::specificity(const Selector& selector) {
        return std::visit([](const auto& simple) -> Specificity {
            using T = std::decay_t<decltype(simple)>;

            if constexpr (std::is_same_v<T, SimpleSelector>) {
                std::size_t a = simple.id.value().size();
                std::size_t b = simple._class.size();
                std::size_t c = simple.tag_name.value().size();
                return {a, b, c};
            }
        }, selector);
}

std::vector<CSS::Selector> CSS::parse_selectors() {
    std::vector<CSS::Selector> selectors{};
    auto condition{true};
    while (condition) {
        selectors.push_back(this->parse_simple_seceltor());
        this->consume_whitespace();
        auto c{this->next_char()};
        switch (c)
        {
        case ',':
            this->consume_char();
            this->consume_whitespace();
            break;

        case '{':
            condition = false;
            break;
        
        default:
            throw std::runtime_error(
                std::format("Unexpected character {} in selector list", c));
        }
    };

    std::sort(selectors.begin(), selectors.end(),
    [&](const Selector& a, const Selector& b) {
        return this->specificity(a) > this->specificity(b); // descending
    });
    return selectors;
}