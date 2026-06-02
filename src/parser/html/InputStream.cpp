#include "InputStream.hpp"
#include <stdexcept>
#include <format>

void InputStream::croak(std::string const& msg) const {
    throw std::runtime_error(std::format("{} ({}:{})", msg, this->_line, this->_col));
}

InputStream::InputStream(std::string const& input): _pos{0}, _line{1}, _col{0}, 
    _input_stream{input} {

};

char InputStream::next(){
    auto ch{this->_input_stream.at(this->_pos++)};
    if (ch == '\n') {
        this->_line++;
        this->_col = 0;
    } else {
        this->_col++;
    }
    return ch;
}

char InputStream::peek() const {
    return this->_input_stream.at(this->_pos);
}

bool InputStream::eof() const {
    return this->_input_stream.size() < this->_pos;
}