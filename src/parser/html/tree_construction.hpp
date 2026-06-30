#pragma once

#include "tokenizer.hpp"
#include <memory>
#include <vector>
#include <map>
#include <stack>

enum class InsertionMode {
    Initial,
    BeforeHTML,
    BeforeHead,
    InHead,
    InHeadNoscript,     // optional at first
    AfterHead,
    InBody,
    Text,               // important for <script>, <style>, <title>, etc.
    InTable,            // can implement later
    InTableText,
    InCaption,
    InColumnGroup,
    InTableBody,
    InRow,
    InCell,
    InTemplate,         // advanced
    AfterBody,
    InFrameset,
    AfterFrameset,
    AfterAfterBody,
    AfterAfterFrameset
};

enum class NodeType {
    Document,
    Element,
    Text,
    Comment,
    Doctype
};


struct Node {
    std::shared_ptr<Node> parent;
    NodeType type;
    std::vector<std::unique_ptr<Node>> children;
};

struct Element : public Node {
    std::string tag_name;                    
    std::map<std::string, std::string> attributes;
};

struct TextNode : public Node {
    std::string data;
};

class TreeConstructor {
    std::unique_ptr<Node> document;
    std::stack<std::shared_ptr<Element> /* shared or unique? */> open_elements;
    InsertionMode insertion_mode = InsertionMode::Initial;
    Element* head_pointer = nullptr;

    void process_token(const Token& token);   // main dispatcher

    // Helper functions you will need:
    void insert_element(const Tokens::TagToken& tag);
    void insert_character(char ch);
    void insert_comment(...);
    void pop_until(const std::string& tag_name);
    bool stack_has(const std::string& tag_name) const;
};