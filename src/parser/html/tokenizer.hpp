#pragma once

#include "InputStream.hpp"
#include <memory>
#include <functional>

class Tokenizer{
    std::unique_ptr<InputStream> const _stream;
    std::string consume_while(std::function<bool(char)>const&) const;
    public:
    Tokenizer(std::string const& txt);
    void remove_spaces() const;
    std::string consume_word();
};