//
// Created by roel on 10/11/22.
//


#pragma once

#include "Source.h"

#include <string>
#include <list>
#include <stdexcept>

class StringSource : public Source {
public:
    explicit StringSource(std::string source);

    [[nodiscard]] bool has_more() const override;

    [[nodiscard]] char peek() const override;

    char next() override;

    class Location : public Source::Location {
    public:
        Location(const StringSource &source_, int start_of_line_, int line_, int col_) :
                source(source_),
                start_of_line(start_of_line_),
                line(line_),
                col(col_) {}


        [[nodiscard]] const Source &get_source() const override {
            return source;
        }

        [[nodiscard]] std::string annotate(const std::string &msg) const override;

    private:
        const StringSource &source;
        const int start_of_line;
        const int line;
        const int col;
    };

    [[nodiscard]] const Location &get_location() override;

private:
    const std::string raw_str;
    unsigned int index;
    unsigned int current_start_of_line;
    unsigned int current_line;
    unsigned int current_col;

    std::list<Location> location_markers;
};


