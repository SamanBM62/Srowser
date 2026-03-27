#pragma once
#include <vector>
#include <unordered_map>
#include <string>



template<typename NodeType>
struct Node{
    std::vector<Node<NodeType>> children;
    NodeType node_data;
};


struct ElementData {
    using AttrMap = std::unordered_map<std::string, std::string>;

    std::string tag_name;
    AttrMap attrs;
};

template<typename NodeType>
Node<NodeType> elem(std::string tag_name, ElementData::AttrMap attr);

#include "DOM.cpp"
