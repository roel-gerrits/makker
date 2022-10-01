//
// Created by roel on 5/30/22.
//


#pragma once

#include <list>
#include <string>
#include "lexer/Token.h"

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
//    explicit Node(NodeType type);

    Node(NodeType type, Token data);

    void add_child(Node &&child);

    [[nodiscard]] NodeType get_type() const;

    [[nodiscard]] const Token &get_token() const;

    [[nodiscard]] const std::list<Node> &get_children() const;

    [[nodiscard]] const Node &get_child(unsigned int index) const;

private:
    const NodeType type;
    const Token token;
    std::list<Node> children;
};

