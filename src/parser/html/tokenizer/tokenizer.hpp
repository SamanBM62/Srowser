#pragma once

#include "InputStream.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <variant>
#include "../tree_constructor/tree_construction.hpp"
#include "token.hpp"

enum class StateMachine {
    data,
    open_tag,
    end_tag_open,
    tag_name
};


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
    const std::unique_ptr<TreeConstructor>& _tree;
    public:
    void main_loop();
    void remove_spaces() const;
    Tokenizer(std::string const& txt, const std::unique_ptr<TreeConstructor>&);
    std::string consume_word();
};