//
// Created by roel on 7/17/22.
//


#pragma once

#include <stdexcept>

#include "parser/Ast.h"
#include "Environment.h"


class InterpretResult {
public:
    [[nodiscard]] bool success() const;

    class Error {
    public:
        Error(const Node &node, std::string message);

        const Node &node;
        const std::string message;
    };

    [[nodiscard]] const std::list<Error> &errors() const;

    void add_error(const Node &node, std::string message);

private:
    std::list<Error> error_list;
};

class Interpreter {
public:
    explicit Interpreter(ObjectStore &object_store, Environment &environment, const Node &ast);

    InterpretResult interpret();

private:
    ObjectStore &object_store;
    Environment &environment;
    InterpretResult result;
    const Node &ast;

    void interpret_program(const Node &node);

    void interpret_statement(const Node &node);

    void interpret_call_statement(const Node &node);

    void interpret_assignment_statement(const Node &node);

    const Object &parse_expression(const Node &node);

    const Object &parse_function_call(const Node &node);
};

InterpretResult interpret(ObjectStore &object_store, Environment &env, const Node &ast);

class InterpretError : public std::exception {
public:
    explicit InterpretError() = default;
};
