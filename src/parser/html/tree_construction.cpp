#include "tree_construction.hpp"
#include "tokenizer.hpp"
#include <memory>
#include <variant>
#include <iostream>

TreeConstructor::TreeConstructor(): document{std::make_shared<Node>()},open_elements{}, insertion_mode{InsertionMode::Initial}, 
    _reprocess{false}, _frame_set_ok{true} {

}

void TreeConstructor::process_initial() {
    //TODO: right now we only apply the anything else part here! implement the rest according to whatwg
    //Anything else: switch the insertion mode to "before html", then reprocess the token.

    this->insertion_mode = InsertionMode::BeforeHTML;
    this->_reprocess = true;
}

void TreeConstructor::process_token(const Token& token) {
    do {

        if (this->_reprocess)
            this->_reprocess = false;

        switch (this->insertion_mode) {
            case InsertionMode::Initial:
                this->process_initial();
                break;
            default:
                break;  
        
        }

    } while (this->_reprocess);
}

void TreeConstructor::process_before_html() {
    /*TODO: implement the rest of the cases according to WHATWG*/

    if (auto* p = std::get_if<char>(&this->_current_token)) {
        if (this->is_html_whitespace(*p))
            //ignore the token
            return;
    }

    if (auto* p = std::get_if<Tokens::TagToken>(&this->_current_token)) {
        if (!p->is_close && p->tag_name == "html") {
            /* Remember to impelent the fucking attributes!!! 13.2.5.33 Attribute name state*/
            auto elem = std::make_shared<Element>();
            elem->tag_name = p->tag_name;
            elem->parent = this->document;
            this->document->children.push_back(elem);

            this->open_elements.push_back(elem);

            this->insertion_mode = InsertionMode::BeforeHead;
            return;
        }

        else if (p->is_close) {
            if(p->tag_name != "head" || p->tag_name != "body" 
                || p->tag_name != "html" || p->tag_name != "br") {

                std::cout << "parse error ignoring token" << std::endl;
                return;
            }
        }
    }

    //anything else case
    auto elem = std::make_shared<Element>();
    elem->tag_name = "html";
    elem->parent = this->document;
    this->document->children.push_back(elem);

    this->open_elements.push_back(elem);

    this->insertion_mode = InsertionMode::BeforeHead;

    this->_reprocess = true;
    return;

}

bool TreeConstructor::is_html_whitespace(char32_t c) const {
    return c == U'\t' ||
           c == U'\n' ||
           c == U'\f' ||
           c == U'\r' ||
           c == U' ';
}

void TreeConstructor::process_before_head() {

    if (auto* p = std::get_if<char>(&this->_current_token)) {
        if (this->is_html_whitespace(*p))
            //ignore the token
            return;
    }

    if (auto* p = std::get_if<Tokens::TagToken>(&this->_current_token)) {
        if (!p->is_close && p->tag_name == "html") {
            /* Process the token using the rules for the "in body" insertion mode. */
            return;
        }

        if (!p->is_close && p->tag_name == "head") {

            // Insert an HTML element for the token.
            auto html  = this->create_elem_push("html", this->document);

            // Set the head element pointer to the newly created head element.
            
            this->head_ptr = this->create_elem_push(p->tag_name, html);

            this->insertion_mode = InsertionMode::InHead;

            return;
        }
    }

    // Insert an HTML element for the token.
    auto html  = this->create_elem_push("html", this->document);

    // Set the head element pointer to the newly created head element.
    
    this->head_ptr = this->create_elem_push("head", html);

    this->insertion_mode = InsertionMode::InHead;
    //Reprocess the current token.
    this->_reprocess = true;

    return;

}

std::shared_ptr<Element> TreeConstructor::create_elem_push(std::string const& tag_name, std::shared_ptr<Node> parent) {

    auto elem = std::make_shared<Element>();
    elem->tag_name = tag_name;
    elem->parent = parent;
    parent->children.push_back(elem);

    this->open_elements.push_back(elem);

    return elem;
}

void TreeConstructor::process_in_head() {
    if (auto* p = std::get_if<char>(&this->_current_token)) {
        if (this->is_html_whitespace(*p)) {
            this->insert_character( *p);
            return;
        }
    }

    if (auto* p = std::get_if<Tokens::TagToken>(&this->_current_token)) {
        if (!p->is_close && p->tag_name == "html") {
            /* Process the token using the rules for the "in body" insertion mode. */
            return;
        }

        if (p->is_close && p->tag_name == "head") {
            this->open_elements.pop_back();
            this->insertion_mode = InsertionMode::AfterHead;
        }

        if ((p->is_close && !(p->tag_name == "body" || p->tag_name == "html" || p->tag_name == "br"))
                 || (!p->is_close && p->tag_name == "head"))
                //Parse error. Ignore the token.
                return;
        
    }

    this->open_elements.pop_back();
    this->insertion_mode = InsertionMode::AfterHead;
    this->_reprocess = true;

    return;
}

void TreeConstructor::insert_character(char c) {
    /* for now peak of open elements is the appropiate place for character insertaion. */

    auto peak {this->open_elements.back()};

    std::shared_ptr<TextNode> found_text {nullptr};

    for (const auto& child : peak->children)
    {
        if (auto text = std::dynamic_pointer_cast<TextNode>(child))
        {
            found_text = text;
            break;
        }
    }

    if (found_text) {
        found_text->data.push_back(c);

    } else {
        auto text{std::make_shared<TextNode>()};
        text->data.push_back(c);
        peak->children.push_back(text);
    }
}

void TreeConstructor::process_after_head() {
    if (auto* p = std::get_if<char>(&this->_current_token)) {
        if (this->is_html_whitespace(*p)) {
            this->insert_character( *p);
            return;
        }
    }

    if (auto* p = std::get_if<Tokens::TagToken>(&this->_current_token)) {
        if (!p->is_close && p->tag_name == "html") {
            /* Process the token using the rules for the "in body" insertion mode. */
            return;
        }

        if (!p->is_close && p->tag_name == "body") {
            auto peak {this->open_elements.back()};
            this->create_elem_push("body", peak);
            this->_frame_set_ok = false;
            this->insertion_mode = InsertionMode::InBody;
            return;
        }

        if ((p->is_close && !(p->tag_name == "body" || p->tag_name == "html" || p->tag_name == "br"))
                 || (!p->is_close && p->tag_name == "head"))
                //Parse error. Ignore the token.
                return;
    }

    auto peak {this->open_elements.back()};
    this->create_elem_push("body", peak);
    this->insertion_mode = InsertionMode::InBody;
    this->_reprocess = true;

}

void TreeConstructor::process_in_body() {
    /* TODO: there are cases where the implementation is dependent on attributes, where I can implemenlt.
         implement them once attribute has been added. */
    if (auto* p = std::get_if<char>(&this->_current_token)) {
        if (*p == '\0')
        //parse error ignore token
            return;

        /* TODO:?? Reconstruct the active formatting elements, if any. */
        this->insert_character(*p);
        if (!this->is_html_whitespace(*p)) {
            this->_frame_set_ok = false;
        }
        return;
    }

    if (auto* p = std::get_if<Tokens::TagToken>(&this->_current_token)) {
        if (!p->is_close && p->tag_name == "html") {
            /* If there is a template element on the stack of open elements, then ignore the token.

            Otherwise, for each attribute on the token, check to see if the attribute is already present on the top element of the stack of open elements.
             If it is not, add the attribute and its corresponding value to that element. */
            return;
        }

        if ((!p->is_close && (p->tag_name == "base" || p->tag_name == "basefont" || p->tag_name == "bgsound" || p->tag_name == "link" 
                || p->tag_name == "meta" || p->tag_name == "noframes" || p->tag_name == "script" || p->tag_name == "style" || p->tag_name == "template"
                || p->tag_name == "title")) || (p->is_close && p->tag_name == "template"))
        {
            this->process_in_head();
            return;
        }

        if (!p->is_close && p->tag_name == "body") {
            //parse error
            if (this->open_elements.size() == 1 || this->open_elements[this->open_elements.size() - 2]->tag_name != "body" /* or if one another condition */)
                // ignore the token;
                return;
            else {
                this->_frame_set_ok = false;
            }
            
        }

        if (p->is_close && (p->tag_name == "body" || p->tag_name == "html")) {
            /* some rules on checking when to ignore a token */
            auto does_have_body_elem{false};
            auto is_specific_elem{false}; //TODO: implement this part as well

            for (const auto& elem: this->open_elements) {
                if (elem->tag_name == "body")
                    does_have_body_elem = true;
            }

            if (!does_have_body_elem) {
                //parse error, ignore token
                return;
            }

            this->insertion_mode = InsertionMode::AfterBody;

            if (p->tag_name == "html")
                this->_reprocess = true;
            return;
        }
        
    }

    // TODO: implement EOF token and its handeling here.
    return;


}

void TreeConstructor::process_after_body() {
    if (auto* p = std::get_if<char>(&this->_current_token)) {
        if (this->is_html_whitespace(*p)) {
            this->process_in_body();
            return;
        }
    }

    if (auto* p = std::get_if<Tokens::TagToken>(&this->_current_token)) {
        if (!p->is_close && p->tag_name == "html") {
            this->process_in_body();
            return;
        }

        if (p->is_close && p->tag_name == "html") {
            this->insertion_mode = InsertionMode::AfterAfterBody;
            return;
        }
    }

    // anything else parse error
    this->insertion_mode = InsertionMode::InBody;
    this->_reprocess = true;
    return;
}

void TreeConstructor::process_after_after_body() {
    if (auto* p = std::get_if<char>(&this->_current_token)) {
        if (this->is_html_whitespace(*p)) {
            this->process_in_body();
            return;
        }
    }

    if (auto* p = std::get_if<Tokens::TagToken>(&this->_current_token)) {
        if (!p->is_close && p->tag_name == "html") {
            this->process_in_body();
            return;
        }
    }
}