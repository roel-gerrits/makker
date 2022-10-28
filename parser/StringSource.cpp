//
// Created by roel on 10/11/22.
//

#include "StringSource.h"

StringSource::StringSource(std::string str_) :
        raw_str(std::move(str_)),
        index(0),
        current_start_of_line(index),
        current_line(1),
        current_col(1) {}

bool StringSource::has_more() const {
    return index < raw_str.size();
}

char StringSource::peek() const {
    return raw_str.at(index);
}

char StringSource::next() {
    char ch = raw_str.at(index);
    if (ch == '\n') {
        current_line++;
        current_col = 0;
        current_start_of_line = index;
    }
    current_col++;
    index++;
    return ch;
}

const StringSource::Location &StringSource::get_location() {
    return location_markers.emplace_back(*this, current_start_of_line, current_line, current_col);
}

std::string StringSource::Location::annotate(const std::string &msg) const {

//    return "[" + std::to_string(line) + ":" + std::to_string(col) + "]";

//    char buf[128];
//    snprintf(buf, sizeof(buf), "StringSource[%u:%u]", line, col);
//    return buf;

//    printf("annotate \n");
    std::string full_line;
    for (unsigned int i = start_of_line; i < source.raw_str.length(); i++) {
        if (source.raw_str.at(i) == '\n')break;
        full_line += source.raw_str.at(i);
    }
//    printf("full_line = %s \n", full_line.c_str());


    std::string str =  /*"[" + std::to_string(line) + ":" + std::to_string(col) + "] " + */ full_line + '\n';
    str += std::string(col - 1, ' ') + "^-- " + msg;


    return str;
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

