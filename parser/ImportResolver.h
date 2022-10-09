//
// Created by roel on 10/6/22.
//


#pragma once

#include "lexer/Token.h"
#include <stdexcept>
#include <utility>

class ImportResolver {
public:
    virtual TokenStream &resolve(const std::string &import_spec) = 0;
};


class ImportError : public std::runtime_error {
public:
    explicit ImportError(std::string import_spec_) :
            std::runtime_error("Failed to import '" + import_spec_ + "'"),
            import_spec(std::move(import_spec_)) {}

private:
    const std::string import_spec;
};