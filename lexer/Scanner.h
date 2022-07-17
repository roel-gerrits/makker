//
// Created by roel on 5/30/22.
//

#pragma once

#include <string>

class Position {
public:
    Position(unsigned int line, unsigned int col) :
            line(line), col(col) {}

    Position(const Position &src) = default;

    Position &operator=(Position &&src) = default;

    unsigned int line;
    unsigned int col;
};

class Scanner {
public:

    [[nodiscard]] virtual bool has_more() const = 0;

    [[nodiscard]] virtual char peek() const = 0;

    virtual char next() = 0;

    [[nodiscard]] virtual Position get_position() const = 0;

    [[nodiscard]] virtual std::string get_line(const Position &position) const = 0;
};