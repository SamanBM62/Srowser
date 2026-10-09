#include "tokenizer/tokenizer.hpp"
#include "tree_constructor/tree_construction.hpp"
#include <memory>

class Parser {
    std::unique_ptr<TreeConstructor> _tree;
    std::unique_ptr<Tokenizer> _tokenizer;
public:
    Parser(std::string txt);
    //temp
    void main_loop();
};