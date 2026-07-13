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
    std::weak_ptr<Node> parent;
    NodeType type;
    std::vector<std::shared_ptr<Node>> children;
};

struct Element : public Node {
    /*TODO: write constructors after adding attributes, and update the code*/
    std::string tag_name;                    
    std::map<std::string, std::string> attributes;
};

struct TextNode : public Node {
    std::string data;
};

// struct Document: public Node {
//     std::string title;
//     std::shared_ptr<Element> head_elemet;
//     std::shared_ptr<Element> body_elemet;
//     std::shared_ptr<Element> html_elemet;
// };

class TreeConstructor {
    std::shared_ptr<Node> document;
    std::vector<std::shared_ptr<Element> /* shared or unique? */> open_elements;
    InsertionMode insertion_mode;
    TreeConstructor();

    void process_token(const Token& token);   // main dispatcher

    
    void pop_until(const std::string& tag_name);
    bool stack_has(const std::string& tag_name) const;
private:
    void process_initial();
    void process_before_html();
    bool is_html_whitespace(char32_t c) const;
    void process_before_head();
    std::shared_ptr<Element> create_elem_push(std::string const& tag_name, std::shared_ptr<Node> parent);
    void process_in_head();
    void insert_character(char c);
    void process_after_head();
    void process_in_body();
    void process_after_body();
    void process_after_after_body();
    
    Token _current_token;
    bool _reprocess;
    std::shared_ptr<Node> head_ptr;
    bool _frame_set_ok;
};