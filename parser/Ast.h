//
// Created by roel on 5/30/22.
//


#pragma once

#include <list>
#include <string>

enum class NodeType {
    PROGRAM,
    STATEMENT_LIST,
    ASSIGNMENT_STATEMENT,
    CALL_STATEMENT,
    EXPR,
    LIST,
    LIST_FOR,
    OBJECT,
    VARIABALE,
    STRING,
};

const char *to_str(NodeType type);

class Node {
public:
    explicit Node(NodeType type);

    Node(NodeType type, std::string data);

    void add_child(Node &&child);

    [[nodiscard]] NodeType get_type() const;

    [[nodiscard]] const std::string &get_data() const;

    [[nodiscard]] const std::list<Node> &get_children() const;

private:
    const NodeType type;
    const std::string data;
    std::list<Node> children;
};

