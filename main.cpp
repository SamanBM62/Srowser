#include <iostream>
#include "DOM.hpp"
#include <string>

int main() {

    using namespace std::string_literals;

    auto root {elem<ElementData>("root"s, ElementData::AttrMap{{"headLine", "Hello"}})};

    auto firstChild{elem<ElementData>("child"s, ElementData::AttrMap{{"headLine", "Hello back"}})};

    root.children.push_back(firstChild);

    std::cout << root.node_data.tag_name << std::endl;

    for (int i {0}; i < root.children.size(); i++) {
        std::cout << std::string(i + 1, '\t') << root.children[i].node_data.tag_name << std::endl; 
    }

    return 0;
}