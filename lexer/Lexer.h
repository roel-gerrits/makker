//
// Created by roel on 5/24/22.
//


#pragma once

#include <stdexcept>

#include "Token.h"
#include "Scanner.h"


class Lexer : public TokenStream{
public:
    explicit Lexer(Scanner &scanner);

    Token next() override;

private:
    Scanner &scanner;

    bool check(char ch);
    void expect(char ch);
};


class UnexpectedCharacter : public std::runtime_error {
public:
    UnexpectedCharacter(char ch, Position pos_) :
            std::runtime_error("Unexpected character '" + std::string(1, ch) + "' at " + std::to_string(pos_.line) + ":" + std::to_string(pos_.col) + ""),
            pos(pos_) {}

    [[nodiscard]] Position get_position() const { return pos; };
private:
    const Position pos;
};

class UnexpectedEnd : public std::runtime_error {
public:
    explicit UnexpectedEnd(Position pos) :
            std::runtime_error("Unexpected end") {}
};
