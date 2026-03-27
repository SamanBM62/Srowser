#include "DOM.hpp"

template<typename NodeType>
Node<NodeType> elem(std::string tag_name, ElementData::AttrMap attr) {
    ElementData elem{tag_name, attr};
    return Node<NodeType>{std::vector<Node<NodeType>>{}, elem};
};


