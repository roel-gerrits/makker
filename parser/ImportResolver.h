//
// Created by roel on 10/11/22.
//

#pragma once

#include "Source.h"
#include <optional>
#include <string>

class ImportResolver {
public:

    class Result {
    public:

        enum class Type {
            MKR_PROGRAM,
            EXTERNAL_OBJECT,
            ERROR,
        };

        Result(Type type_, Source &source_) :
                _type(type_),
                source(source_) {}

        explicit Result(Type type_) :
                _type(type_),
                source() {}

        bool success() { return _type != Type::ERROR; };

        class Error {

        };

        Type type() { return _type; }

        Source &get_source() {
            return source.value();
        }

    private:
        const Type _type;
        std::optional<std::reference_wrapper<Source>> source;
    };

    virtual Result resolve(const std::string &import_spec) = 0;
};