//
// Created by roel on 5/24/22.
//


#pragma once

#include <stdexcept>

#include "Token.h"
#include "Source.h"


class Lexer : public TokenStream {
public:
    explicit Lexer(Source &source);

    Token next() override;

private:
    Source &source;

    bool check(char ch);

    void expect(char ch);
};


class UnexpectedCharacter : public std::runtime_error {
public:
    UnexpectedCharacter(char ch, const Source::Location &location_) :
            std::runtime_error("Unexpected character '" + std::string(1, ch)),
            location(location_) {}

    [[nodiscard]] const Source::Location &get_position() const { return location; };

private:
    const Source::Location &location;
};

class UnexpectedEnd : public std::runtime_error {
public:
    explicit UnexpectedEnd(const Source::Location& pos) :
            std::runtime_error("Unexpected end") {}
};
