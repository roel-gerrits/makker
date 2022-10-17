//
// Created by roel on 6/17/22.
//


#pragma once

#include <list>

#include "parser/Token.h"

class RewindableTokenStream {
public:
    explicit RewindableTokenStream(TokenStream &source);

    const Token &peek();

    const Token &next();

    class Snapshot {
        friend RewindableTokenStream;
    public:
        explicit Snapshot(unsigned int index);

    private:
        unsigned int index;
    };

    Snapshot snapshot();

    void rewind(Snapshot snapshot);

private:
    TokenStream &source;
    std::list<Token> buffer;
    std::list<Token>::iterator buffer_pos;

    unsigned int buffer_start_index;
    unsigned int buffer_current_index;

    void print();
};