#include "DOM.hpp"


dom::Node dom::elem(std::string tag_name, dom::ElementData::AttrMap attr, std::vector<dom::Node> childern) {
    dom::ElementData elem{tag_name, attr};
    return dom::Node{elem, childern};
};


dom::Node::Node(NodeType data): node_data{data}, children{std::vector<dom::Node>{}} {};
dom::Node::Node(NodeType data, std::vector<dom::Node> childern): node_data{data}, children{childern} {};

dom::Node dom::text(std::string data) {
    return dom::Node{data};
}

