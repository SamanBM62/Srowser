#pragma once
#include <string>
#include <variant>

namespace Tokens {
    using CharacterToken = char;
    
    struct TagToken {
        TagToken(bool = false);
        std::string tag_name;
        bool is_close;
        /* TODO:: add the other attributes */
    };
}

using Token = std::variant<Tokens::CharacterToken, Tokens::TagToken>;