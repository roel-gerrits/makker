//
// Created by roel on 5/30/22.
//

#pragma once

#include "Scanner.h"

class StringScanner : public Scanner {
public:
    explicit StringScanner(std::string str);

    bool has_more() const override;

    char peek() const override;

    char next() override;

    Position get_position() const override;

    std::string get_line(const Position &position) const override;

private:
    const std::string str;
    unsigned int index;
    unsigned int line;
    unsigned int col;
};
