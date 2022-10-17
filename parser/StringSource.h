//
// Created by roel on 10/11/22.
//


#pragma once

#include "Source.h"

#include <string>
#include <vector>

class StringSource : public Source {
public:
    explicit StringSource(std::string source);

    [[nodiscard]] bool has_more() const override;

    [[nodiscard]] char peek() const override;

    char next() override;

    class Location : public Source::Location {
    public:
        Location(const StringSource &source_, int line_, int col_) :
                source(source_),
                line(line_),
                col(col_) {}

        [[nodiscard]] const Source &get_source() const override {
            return source;
        }

        [[nodiscard]] std::string describe() const override {
            char buf[128];
            snprintf(buf, sizeof(buf), "StringSource[%u:%u]", line, col);
            return buf;
        }

    private:
        const StringSource &source;
        const int line;
        const int col;
    };

    [[nodiscard]] const Location &get_location() override;

private:
    const std::string str;
    unsigned int index;
    unsigned int line;
    unsigned int col;

    std::vector<Location> location_markers;
};


