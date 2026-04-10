#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <variant> 


namespace dom {
    struct ElementData {
        using AttrMap = std::unordered_map<std::string, std::string>;
    
        std::string tag_name;
        AttrMap attrs;
    };
    
    struct Node{
        private:
        using NodeType = std::variant<dom::ElementData, std::string>;
        public:
        Node (dom::Node::NodeType data);
        Node (dom::Node::NodeType data, std::vector<dom::Node> childern);
        NodeType node_data;
        std::vector<Node> children;
    };

    Node elem(std::string tag_name, ElementData::AttrMap attr, std::vector<dom::Node> childern);

    Node text(std::string data);
}
