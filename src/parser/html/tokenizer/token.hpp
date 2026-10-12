#pragma once
#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <utility>

namespace Tokens {
    using CharacterToken = char;
    
    struct TagToken {
        TagToken(bool = false);
        TagToken(TagToken const&) = delete;
        TagToken(TagToken&&) = default;
        std::string tag_name;
        bool is_close;
        std::vector<std::unique_ptr<std::pair<std::string, std::string>>> attributes{};
    };
}

using Token = std::variant<Tokens::CharacterToken, Tokens::TagToken>;