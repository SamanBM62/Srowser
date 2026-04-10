#include <iostream>
#include "DOM.hpp"
#include "parser.hpp"
#include <string>
#include <fstream>
#include <sstream>

int main() {

    using namespace std::string_literals;

    std::ifstream file{"page.html"};

    std::ostringstream ss{};
    ss << file.rdbuf();

    std::string input {ss.str()};

    Parser pars{0, input};
    auto nodes {pars.parse(input)};

    // std::cout << nodes << std::endl;
    return 0;
}