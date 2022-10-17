//
// Created by roel on 10/11/22.
//

#include "StringSource.h"

StringSource::StringSource(std::string str_) :
        str(std::move(str_)),
        index(0),
        line(1),
        col(1) {}

bool StringSource::has_more() const {
    return index < str.size();
}

char StringSource::peek() const {
    return str.at(index);
}

char StringSource::next() {
    char ch = str.at(index);
    if (ch == '\n') {
        line++;
        col = 0;
    }
    col++;
    index++;
    return ch;
}

const StringSource::Location &StringSource::get_location() {
    return location_markers.emplace_back(*this, line, col);
}

//std::string StringSource::get_line(const Position &position) const {
//    unsigned int cur_line = 1;
//    std::string line;
//    for (char c: str) {
//        if (c == '\n') {
//            if (cur_line == position.line) {
//                break;
//            }
//            line.clear();
//            cur_line++;
//        } else {
//            line += c;
//        }
//    }
//    return line;
//}

