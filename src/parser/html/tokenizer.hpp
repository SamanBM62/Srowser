#pragma once

#include "InputStream.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <variant>

enum class StateMachine {
    data,
    open_tag,
    end_tag_open,
    tag_name
};

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
class Tokenizer{

    std::unique_ptr<InputStream> const _stream;
    std::string consume_while(std::function<bool(char)>const&) const;

    /* since the state machine is huge!! for maintainablity is better to use this rather than switch.
    * however, it can also be implemented with arrays. if things are slow alternetive solutions should be explored.
    */
    StateMachine _state;
    std::unordered_map<StateMachine, std::function<void()>> _main_executer;

    void open_state();
    void tag_open_state();
    void tag_name_state();
    void end_tag_open_state();

    Token _current_token;

    public:
    void main_loop();
    void remove_spaces() const;
    Tokenizer(std::string const& txt);
    std::string consume_word();
};