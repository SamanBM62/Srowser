#include "tokenizer.hpp"
#include <cctype>
#include <iostream>

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

Tokenizer::Tokenizer(std::string const& txt): _stream{new InputStream{txt}}, _state{StateMachine::data} {
    this->_main_executer = {
        {StateMachine::data, this->open_state()},
        {StateMachine::end_tag_open, this->end_tag_open_state()},
        {StateMachine::open_tag, this->tag_open_state()},
        {StateMachine::end_tag_open, this->end_tag_open_state()}
    };
}

void Tokenizer::main_loop() {
    while (!this->_stream->eof())
        this->_main_executer[this->_state]();
}

void Tokenizer::open_state() {
    auto ch{this->_stream->next()};

    switch (ch) {
        case '&':
        //TODO: implement according to the standard
        break;
        case '<':
            this->_state = StateMachine::open_tag;
        break;
        //TODO: handle the EOF token and null case
        default:
            this->_current_token = ch;
            std::cout << "emitting token character: " << std::get<Tokens::CharacterToken>(this->_current_token) << std::endl;
            break;
    }
}

void Tokenizer::tag_open_state() {
    auto ch {this->_stream->peek()};

    if (std::isalpha(static_cast<unsigned char>(ch)))
        {
            this->_current_token = Tokens::TagToken{};
            this->_state = StateMachine::tag_name;
        }

    switch (ch) {
        case '/':
            this->_state = StateMachine::end_tag_open;
            this->_stream->next();
        break;
        /*TODO: implemet rest according to the standards*/
    
    }
}

void Tokenizer::tag_name_state() {
    auto ch{this->_stream->next()};
    if (std::isalpha(static_cast<unsigned char>(ch)))
        {
            if (std::isupper(static_cast<unsigned char>(ch)))
                ch = std::tolower(static_cast<unsigned char>(ch));
            
            std::get<Tokens::TagToken>(this->_current_token).tag_name += ch;
        }
    switch (ch) {
        case '>':
            this->_state = StateMachine::data;
            std::cout << "emitting current tag token: " << std::get<Tokens::TagToken>(this->_current_token).tag_name << std::endl;
        break;
        /*TODO: implemet rest according to the standards*/
    }
}

void Tokenizer::end_tag_open_state() {
    auto ch {this->_stream->peek()};

    if (std::isalpha(static_cast<unsigned char>(ch)))
        {
            this->_current_token = Tokens::TagToken{};
            this->_state = StateMachine::tag_name;
        }

    switch (ch) {
        case '>':
            this->_state = StateMachine::data;
            this->_stream->next();
        break;
        /*TODO: implemet rest according to the standards*/
    
    }
}