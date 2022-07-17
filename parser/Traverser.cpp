//
// Created by roel on 6/18/22.
//

#include "Traverser.h"

Traverser::Traverser(const Node &root) :
        root(root) {}

Node Traverser::next() {
    return Node(NodeType::ASSIGNMENT_STATEMENT);
}
