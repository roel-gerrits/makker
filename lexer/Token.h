//
// Created by roel on 5/30/22.
//

#pragma once

#include <string>
#include "Scanner.h"

enum class TokenType {
    IDENTIFIER,
    STRING,
    DOT,
    BRACK_OPEN,
    BRACK_CLOSE,
    PAR_OPEN,
    PAR_CLOSE,
    ASSIGN,
    FOR,
    IN,
    EOS,
};

const char * to_str(TokenType t);

std::ostream &operator<<(std::ostream &os, const TokenType &t);

class Token {
public:
    Position pos;
    TokenType type;
    std::string value;

    Token(Position pos_, TokenType type_, std::string value_) :
            pos(pos_),
            type(type_),
            value(std::move(value_)) {
    }

    Token(Position pos_, TokenType type_) :
            pos(pos_),
            type(type_),
            value() {
    }
};

class TokenStream {
public:
    virtual Token next() = 0;
};