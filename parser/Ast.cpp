//
// Created by roel on 5/30/22.
//

#include "Ast.h"

#include <utility>

const char *to_str(NodeType type) {
    switch (type) {
        case NodeType::PROGRAM:
            return "PROGRAM";
        case NodeType::STATEMENT_LIST:
            return "STATEMENT_LIST";
        case NodeType::ASSIGNMENT_STATEMENT:
            return "ASSIGNMENT_STATEMENT";
        case NodeType::CALL_STATEMENT:
            return "CALL_STATEMENT";
        case NodeType::EXPR:
            return "EXPR";
        case NodeType::LIST:
            return "LIST";
        case NodeType::LIST_FOR:
            return "LIST_FOR";
        case NodeType::OBJECT:
            return "OBJECT";
        case NodeType::VARIABALE:
            return "VARIABLE";
        case NodeType::STRING:
            return "STRING";
        default:
            return "???";
    }
}

Node::Node(NodeType type) :
        Node(type, {}) {}

Node::Node(NodeType type, std::string data) :
        type(type),
        data(std::move(data)),
        children() {}

void Node::add_child(Node &&child) {
    children.push_back(std::move(child));
}

NodeType Node::get_type() const {
    return type;
}

const std::string &Node::get_data() const {
    return data;
}

const std::list<Node> &Node::get_children() const {
    return children;
}
