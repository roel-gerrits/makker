//
// Created by roel on 5/30/22.
//


#pragma once

#include <list>
#include <string>
#include "parser/Source.h"

enum class NodeType {
    PROGRAM,
    ASSIGNMENT_STATEMENT,
    CALL_STATEMENT,
    KWARG,
    LIST,
    LIST_FOR,
    OBJECT,
    VARIABLE,
    STRING,
};

const char *to_str(NodeType type);

class Node {
public:
    Node(NodeType type, const Source::Location &source_location);

    Node(NodeType type, const Source::Location &source_location, std::string data);

    void add_child(Node &&child);

    [[nodiscard]] NodeType get_type() const;

    [[nodiscard]] const Source::Location &get_source_location() const;

    [[nodiscard]] const std::string& get_data() const;

    [[nodiscard]] const std::list<Node> &get_children() const;

    [[nodiscard]] const Node &get_child(unsigned int index) const;

private:
    const NodeType type;
    const Source::Location &source_location;
    std::string data;
    std::list<Node> children;
};

