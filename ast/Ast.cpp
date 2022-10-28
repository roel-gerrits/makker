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


Node::Node(NodeType type_, const Source::Location &source_location_) :
        type(type_),
        source_location(source_location_),
        data(),
        children() {}

Node::Node(NodeType type_, const Source::Location &source_location_, std::string data_) :
        type(type_),
        source_location(source_location_),
        data(std::move(data_)),
        children() {}


void Node::add_child(Node &&child) {
    children.push_back(std::move(child));
}

NodeType Node::get_type() const {
    return type;
}

const Source::Location &Node::get_source_location() const {
    return source_location;
}

const std::string &Node::get_data() const {
    return data;
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
