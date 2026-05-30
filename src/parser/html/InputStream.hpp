#pragma once
#include <cstddef>
#include <string>

//TODO: make sure to see if it is possible to intergrate it with c++ streams!

class InputStream {
    std::size_t _pos, _line, _col;
    std::string _input_stream;
    
    void croak(std::string const& msg) const;
    public:
    InputStream(std::string const& input);
    
    char next();
    char peek() const;
    bool eof() const;

};