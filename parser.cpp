#include "parser.hpp"
#include <stdexcept>
#include <cctype>
#include <cassert>
#include <format>

char Parser::next_char() const{
    return this->input[this->pos];
}

bool Parser::starts_with(std::string const& s) const {
    return std::string_view(this->input).substr(this->pos).starts_with(s);
}

void Parser::expect(std::string const& s) {
    if (this->starts_with(s))
        this->pos += s.length();
    else
        throw std::runtime_error(
        std::format("Expected {} at byte {} but it was not found", s, this->pos)
    );
}

char Parser::consume_char() {
    auto ret{this->next_char()};
    this->pos++;
    return ret;
}

bool Parser::eof() {
    return this->pos >= this->input.length();
}

std::string Parser::consume_while(std::function<bool(char)> test) {
    std::string result{};

    while (!this->eof() && test(this->next_char()))
        result.push_back(this->consume_char());
    
    return result;
} 

void Parser::consume_whitespace() {
    this->consume_while([](char c){
        return std::isspace(static_cast<unsigned char>(c));
    });
}

std::string Parser::parse_name() {
    return this->consume_while([](char c){
        return std::isalnum(static_cast<unsigned char>(c));
    });
}

dom::Node Parser::text(std::string data) {
    return dom::Node {data};
}

dom::Node Parser::parse_text() {
    return dom::text(this->consume_while([](char c){ return c != '<'; }));
};


dom::Node Parser::parse_element() {
    using namespace std::string_literals;
    this->expect("<"s);
    auto tag_name {this->parse_name()};
    auto attrs {this->parse_attributes()};
    this->expect(">"s);

    auto children {this->parse_nodes()};

    this->expect("</"s);
    this->expect(tag_name);
    this->expect(">");

    return dom::elem(tag_name, attrs, children);
}

std::pair<std::string, std::string> Parser::parse_attr() {
    auto name{this->parse_name()};
    this->expect("=");
    auto value{this->parse_attr_value()};

    return {name, value};
}

std::string Parser::parse_attr_value() {
    auto open_quote {this->consume_char()};
    assert( open_quote == '"' || open_quote == '\'');
    auto value {this->consume_while([&open_quote](char c){ return c!= open_quote;})};
    auto close_quote {this->consume_char()};
    assert(open_quote == close_quote);

    return value;
}

dom::ElementData::AttrMap Parser::parse_attributes() {
    dom::ElementData::AttrMap attributes{};

    while (true) {
        this->consume_whitespace();
        if (this->next_char() == '>')
            break;
        auto [name, value] = this->parse_attr();
        attributes[name] = value;
    }


    return attributes;
}

std::vector<dom::Node> Parser::parse_nodes() {
    std::vector<dom::Node> nodes{};

    while (true) {
        this->consume_whitespace();
        if (this->eof() || this->starts_with("</"))
            break;
        nodes.push_back(this->parse_node());
    };

    return nodes;
}

dom::Node Parser::parse_node() {
    if (this->starts_with("<"))
        return this->parse_element();
    else
        return this->parse_text();
}

dom::Node Parser::parse(std::string source) {
    auto nodes{Parser{0, source}.parse_nodes()};
    if (nodes.size() == 1)
        return nodes.front();
    else
        dom::elem("html", dom::ElementData::AttrMap{}, nodes);
    return nodes.front();
}

Parser::Parser(std::size_t pos, std::string input): pos{pos}, input{input}{};

