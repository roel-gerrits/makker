//
// Created by roel on 5/30/22.
//

#include <utility>

#include "StringScanner.h"

StringScanner::StringScanner(std::string str) :
        str(std::move(str)),
        index(0),
        line(1),
        col(1) {
}

bool StringScanner::has_more() const {
    return index < str.size();
}

char StringScanner::peek() const {
    return str.at(index);
}

char StringScanner::next() {
    char ch = str.at(index);
    if (ch == '\n') {
        line++;
        col = 0;
    }
    col++;
    index++;
    return ch;
}

Position StringScanner::get_position() const {
    return {line, col};
}

std::string StringScanner::get_line(const Position &position) const {
    unsigned int cur_line = 1;
    std::string line;
    for (char c: str) {
        if (c == '\n') {
            if (cur_line == position.line) {
                break;
            }
            line.clear();
            cur_line++;
        } else {
            line += c;
        }
    }
    return line;
}
