#include "tokenizer.hpp"


std::string Tokenizer::consume_while(std::function<bool(char)> const& fun) const{
    std::string result{""};
    while(!this->_stream->eof() && fun(this->_stream->peek()))
        result += this->_stream->next();
    return result;
}

void Tokenizer::remove_spaces() const {
    this->consume_while([](char ch){
        return std::isspace(static_cast<unsigned char>(ch));
    });
}

std::string Tokenizer::consume_word() {
    /* learned about copy elision, nice :)*/
    return this->consume_while([](char ch){
        return std::isalpha(static_cast<unsigned char>(ch));
    });
}

Tokenizer::Tokenizer(std::string const& txt): _stream{new InputStream(txt)} {

}