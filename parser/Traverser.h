//
// Created by roel on 6/18/22.
//


#pragma once

#include "Ast.h"

class Traverser {
public:
    Traverser(const Node &root);

    Node next();

private:
    const Node &root;
};


