//
// Created by roel on 5/30/22.
//

#include "Ast.h"

#include <utility>
#include <stdexcept>

const char *to_str(NodeType type) {
    switch (type) {
        case NodeType::PROGRAM:
            return "PROGRAM";
        case NodeType::ASSIGNMENT_STATEMENT:
            return "ASSIGNMENT_STATEMENT";
        case NodeType::CALL_STATEMENT:
            return "CALL_STATEMENT";
        case NodeType::KWARG:
            return "KWARG";
        case NodeType::LIST:
            return "LIST";
        case NodeType::LIST_FOR:
            return "LIST_FOR";
        case NodeType::OBJECT:
            return "OBJECT";
        case NodeType::VARIABLE:
            return "VARIABLE";
        case NodeType::STRING:
            return "STRING";
    }
    return "???";
}

//Node::Node(NodeType type) :
//        Node(type, {}) {}

Node::Node(NodeType type, Token token_) :
        type(type),
        token(std::move(token_)),
        children() {}

void Node::add_child(Node &&child) {
    children.push_back(std::move(child));
}

NodeType Node::get_type() const {
    return type;
}

const Token &Node::get_token() const {
    return token;
}

const std::list<Node> &Node::get_children() const {
    return children;
}

const Node &Node::get_child(unsigned int index) const {
    if (index >= children.size()) {
        throw std::out_of_range("Child index is out of range");
    }
    return *std::next(children.begin(), index);
}
