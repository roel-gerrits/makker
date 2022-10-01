//
// Created by roel on 10/1/22.
//


#pragma once

#include "Object.h"

class Scope {
public:
    [[nodiscard]] virtual const Object &get(const std::string &variable) const = 0;

    virtual void put(const std::string &variable, const Object &object) = 0;

    class UndefinedVariableError : public std::runtime_error {
    public:
        explicit UndefinedVariableError(const std::string &variable) :
                std::runtime_error("Undefined variable '" + variable + "'"),
                variable(variable) {};
    private:
        const std::string variable;
    };

    class AlreadyDefinedError : public std::runtime_error {
    public:
        explicit AlreadyDefinedError(const std::string &variable) :
                std::runtime_error("Variable '" + variable + "' is already defined"),
                variable(variable) {};
    private:
        const std::string variable;
    };
};

