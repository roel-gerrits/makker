//
// Created by roel on 10/11/22.
//

#pragma once

#include <string>

class Source {
public:
    class Location {
    public:
        [[nodiscard]] virtual const Source &get_source() const = 0;

        [[nodiscard]] virtual std::string describe() const = 0;
    };

    [[nodiscard]] virtual bool has_more() const = 0;

    [[nodiscard]] virtual char peek() const = 0;

    virtual char next() = 0;

    [[nodiscard]] virtual const Location &get_location() = 0;
};