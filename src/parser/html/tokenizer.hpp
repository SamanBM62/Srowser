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
        std::string tag_name{};
        /* TODO:: add the other attributes */
    };
}
class Tokenizer{

    std::unique_ptr<InputStream> const _stream;
    std::string consume_while(std::function<bool(char)>const&) const;

    /* since the state machine is huge!! for maintainablity is better to use this rather than switch.
    * however, it can also be implemented with arrays. if things are slow alternetive solutions should be explored.
    */
    std::unordered_map<StateMachine, std::function<void()>> _main_executer;
    StateMachine _state;

    void open_state();
    void tag_open_state();
    void tag_name_state();
    void end_tag_open_state();

    std::variant<Tokens::CharacterToken, Tokens::TagToken> _current_token;

    public:
    void main_loop();
    void remove_spaces() const;
    Tokenizer(std::string const& txt);
    std::string consume_word();
};