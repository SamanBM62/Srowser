#include "parser.hpp"
#include <stdexcept>
#include <cctype>

char Parser::next_char() const{
    return this->input[this->pos];
}

bool Parser::starts_with(std::string& s) const{
    return this->input.starts_with(s);
}

void Parser::expect(std::string& s) {
    if (this->input.starts_with(s))
        this->pos += s.length();
    else
        throw std::runtime_error("Expected {:?} at byte {} but it was not found");
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

bool Parser::is_whitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

void Parser::consume_whitespace() {
    this->consume_while(Parser::is_whitespace);
}