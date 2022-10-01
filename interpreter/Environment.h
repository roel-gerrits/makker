//
// Created by roel on 7/25/22.
//


#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "Object.h"

class Environment {
public:

    void define(const std::string &variable, const Object &obj);

    const Object &get(const std::string &variable);

private:
    std::unordered_map<std::string, const Object &> objects;
};


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