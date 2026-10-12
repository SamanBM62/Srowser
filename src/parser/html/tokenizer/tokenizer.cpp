#include "tokenizer.hpp"
#include <cctype>
#include <functional>
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

Tokenizer::Tokenizer(std::string const& txt, const std::unique_ptr<TreeConstructor>& tree): _stream{new InputStream{txt}}, _state{StateMachine::data}
, _main_executer{
    {StateMachine::data, std::bind(&Tokenizer::open_state, this)},
        {StateMachine::end_tag_open, std::bind(&Tokenizer::end_tag_open_state, this)},
        {StateMachine::open_tag, std::bind(&Tokenizer::tag_open_state, this)},
        {StateMachine::tag_name, std::bind(&Tokenizer::tag_name_state, this)},
        {StateMachine::open_tag, std::bind(&Tokenizer::tag_open_state, this)},
        {StateMachine::before_attribute_name, std::bind(&Tokenizer::before_attribute_state, this)},
        {StateMachine::attribute_name, std::bind(&Tokenizer::attribute_name_state, this)},
        {StateMachine::after_attribute_name, std::bind(&Tokenizer::after_attribute_name, this)},
        {StateMachine::before_attribute_value, std::bind(&Tokenizer::before_attribute_value, this)},
        {StateMachine::attribute_value, std::bind(&Tokenizer::attribute_value, this)}},
        _tree {tree} {
    
}

void Tokenizer::main_loop() {
    while (!this->_stream->eof())
        this->_main_executer[this->_state]();
}

void Tokenizer::open_state() {

    if (this->_stream->eof())
        {
            std::cout << "Emit EOF token" << std::endl;
            return;
        }
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
            this->_tree->process_token(std::move(this->_current_token));
            break;
    }
}

void Tokenizer::tag_open_state() {
    auto ch {this->_stream->peek()};

    if (std::isalpha(static_cast<unsigned char>(ch)))
        {
            this->_current_token.emplace<Tokens::TagToken>();
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
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            this->_state = StateMachine::before_attribute_name;
        break;
        case '>':
            this->_state = StateMachine::data;
            this->_tree->process_token(std::move(this->_current_token));
        break;
        /*TODO: implemet rest according to the standards*/
    }
}

void Tokenizer::end_tag_open_state() {
    auto ch {this->_stream->peek()};

    if (std::isalpha(static_cast<unsigned char>(ch)))
        {
            this->_current_token.emplace<Tokens::TagToken>(true);
            this->_state = StateMachine::tag_name;
        }

    switch (ch) {
        case '!':
        /*TODO: this part should be implemented later, right now it is not necessary*/
        break;
        case '>':
            this->_state = StateMachine::data;
            this->_stream->next();
        break;
        /*TODO: implemet rest according to the standards*/
    
    }
}

void Tokenizer::before_attribute_state() {
    auto ch {this->_stream->peek()};
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        this->_stream->next();
        break;
    case '/':
    case '>':
        this->_state = StateMachine::after_attribute_name;
        break;    
    default:
        std::get<Tokens::TagToken>(this->_current_token).attributes.push_back(std::make_unique<std::pair<std::string, std::string>>("", ""));
        this->_state = StateMachine::attribute_name;
        break;
    }
}

void Tokenizer::attribute_name_state() {
    auto ch {this->_stream->peek()};

    if (std::isalpha(static_cast<unsigned char>(ch)))
    {
        if (std::isupper(static_cast<unsigned char>(ch)))
            ch = std::tolower(static_cast<unsigned char>(ch));
        
        std::get<Tokens::TagToken>(this->_current_token).attributes.back()->first.push_back(ch);
        this->_stream->next();
        return;
    }

    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
    case '/':
    case '>':
        this->_state = StateMachine::after_attribute_name;
        break;
    case '=':
        this->_stream->next();
        this->_state = StateMachine::before_attribute_value;
        break;    
    default:
        std::get<Tokens::TagToken>(this->_current_token).attributes.back()->first.push_back(ch);
        this->_stream->next();
        break;
    }
}

void Tokenizer::after_attribute_name() {
    auto ch {this->_stream->peek()};
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        this->_stream->next();
        break;
    case '>':
        this->_stream->next();
        this->_state = StateMachine::data;
        this->_tree->process_token(std::move(this->_current_token));
        break;
    case '=':
        this->_stream->next();
        this->_state = StateMachine::before_attribute_value;
        break;    
    default:
        std::get<Tokens::TagToken>(this->_current_token).attributes.push_back(std::make_unique<std::pair<std::string, std::string>>("", ""));
        this->_state = StateMachine::attribute_name;
        break;
    }
}

void Tokenizer::before_attribute_value() {

    auto ch {this->_stream->peek()};
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
    case '\'':
    case '"':
        this->_stream->next();
        break;
    case '>':
        this->_state = StateMachine::data;
        this->_tree->process_token(std::move(this->_current_token));
        this->_stream->next();
        break;    
    default:
        this->_state = StateMachine::attribute_value;
        break;
    }

}

void Tokenizer::attribute_value() {
    auto ch {this->_stream->next()};

    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        this->_state = StateMachine::before_attribute_name;
        break;
    case '&':
        /* for now ignore :))*/
        break;
    case '>':
        this->_state = StateMachine::data;
        this->_tree->process_token(std::move(this->_current_token));
        break;    
    default:
        std::get<Tokens::TagToken>(this->_current_token).attributes.back()->second.push_back(ch);
        break;
    }    
}

Tokens::TagToken::TagToken(bool is_close): tag_name{}, is_close{is_close} {
}