//
// Created by roel on 7/17/22.
//


#pragma once

#include <stdexcept>

#include "ast/Ast.h"
#include "Scope.h"


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
    explicit Interpreter(ObjectStore &object_store, Scope &root_scope, const Node &ast);

    InterpretResult interpret();

private:
    ObjectStore &object_store;
    Scope &root_scope;
    InterpretResult result;
    const Node &ast;

    void interpret_program(Scope &scope, const Node &node);

    void interpret_statement(Scope &scope, const Node &node);

    void interpret_call_statement(Scope &scope, const Node &node);

    void interpret_assignment_statement(Scope &scope, const Node &node);

    const Object &parse_expression(Scope &scope, const Node &node);

    const Object &parse_object(Scope &scope, const Node &node);

    const Object &parse_list(Scope &scope, const Node &node);

    const Object &parse_list_for(Scope &scope, const Node &node);

    const Object &parse_function_call(Scope &scope, const Node &node);

    const Object &parse_program(Scope &scope, const Node &node);
};

InterpretResult interpret(ObjectStore &object_store, Scope &root_scope, const Node &ast);

class InterpretError : public std::exception {
public:
    explicit InterpretError() = default;
};

