#include "parser.hpp"

Parser::Parser(std::string txt): _tree{std::make_unique<TreeConstructor>()},_tokenizer{std::make_unique<Tokenizer>(txt, this->_tree)} {
}

void Parser::main_loop() {
    this->_tokenizer->main_loop();
}
