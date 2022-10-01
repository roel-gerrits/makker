//
// Created by roel on 7/17/22.
//

#include "Interpreter.h"

#include <utility>

/*
 * InterpretResult::*
 */

InterpretResult::Error::Error(const Node &node_, std::string message_) :
        node(node_),
        message(std::move(message_)) {}

bool InterpretResult::success() const {
    return error_list.empty();
}

const std::list<InterpretResult::Error> &InterpretResult::errors() const {
    return error_list;
}

void InterpretResult::add_error(const Node &node, std::string message) {
    error_list.emplace_back(node, std::move(message));
}

/*
 * Interpreter::*
 */

Interpreter::Interpreter(ObjectStore &object_store_, Environment &environment_, const Node &ast_) :
        object_store(object_store_),
        environment(environment_),
        ast(ast_) {}


InterpretResult Interpreter::interpret() {
    try {
        interpret_program(ast);
    } catch (const InterpretError &) {

    }
    return result;
}

void Interpreter::interpret_program(const Node &node) {
    for (const Node &child: node.get_children()) {
        interpret_statement(child);
    }
}

void Interpreter::interpret_statement(const Node &node) {
    if (node.get_type() == NodeType::ASSIGNMENT_STATEMENT) {
        interpret_assignment_statement(node);
    } else if (node.get_type() == NodeType::CALL_STATEMENT) {
        interpret_call_statement(node);
    } else {
        result.add_error(node, "Unexpected node '" + std::string(to_str(node.get_type())) + "'");
        throw InterpretError();
    }
}

void Interpreter::interpret_call_statement(const Node &node) {
    parse_function_call(node);
}

void Interpreter::interpret_assignment_statement(const Node &node) {
    const std::string &variable = node.get_child(0).get_token().value;
    const Object &value = parse_expression(node.get_child(1));

    if (NullObject::is_null(value)) {
        result.add_error(node, "Cannot set variable to null");
        throw InterpretError();
    }

    try {
        environment.define(variable, value);
    } catch (const AlreadyDefinedError &) {
        result.add_error(node, "Variable already defined");
        throw InterpretError();
    }
}

const Object &Interpreter::parse_expression(const Node &node) {

    if (node.get_type() == NodeType::OBJECT) {
        const std::string &id = node.get_token().value;

        if (!node.get_children().empty()) {
            return parse_expression(node.get_child(0)).attr(id);
        }

        try {
            return environment.get(id);
        } catch (UndefinedVariableError &) {
            result.add_error(node, "Undefined variable");
            throw InterpretError();
        }
    }

    if (node.get_type() == NodeType::CALL_STATEMENT) {
        return parse_function_call(node);
    }

    if (node.get_type() == NodeType::STRING) {
        const Object &obj = object_store.create_string(node.get_token().value);
        return obj;
    }

    result.add_error(node, "Unexpected node '" + std::string(to_str(node.get_type())) + "'");
    throw InterpretError();
}

const Object &Interpreter::parse_function_call(const Node &node) {
    const Object &function_object = parse_expression(node.get_child(0));

    if (!function_object.is_callable()) {
        result.add_error(node, "Object is not callable");
        throw InterpretError();
    }

    struct Arg : public CallArg {
        explicit Arg(const Node &node_, const Object &obj_) :
                node(node_),
                obj(obj_) {}

        [[nodiscard]] const Object &object() const override {
            return obj;
        }

        const Node &node;
        const Object &obj;
    };

    const CallHandler &call_handler = function_object.get_call_handler();

    std::list<Arg> arg_store;
    CallArgList arg_list;
    for (auto arg_node = ++node.get_children().begin(); arg_node != node.get_children().end(); arg_node++) {
        if (arg_node->get_type() == NodeType::KWARG) {
            arg_list.add(arg_node->get_token().value, arg_store.emplace_back(node, parse_expression(arg_node->get_child(0))));
        } else {
            arg_list.add(arg_store.emplace_back(node, parse_expression(*arg_node)));
        }
    }

    const CallResult call_result = call_handler.call(arg_list);

    for (const CallResult::ArgError &error: call_result.arg_errors()) {
        const Arg &arg = dynamic_cast<const Arg &>(error.arg);
        result.add_error(arg.node, error.message);
    }

    for (const CallResult::CallError &error: call_result.call_errors()) {
        result.add_error(node, error.message);
    }

    if (!call_result.success()) {
        throw InterpretError();
    }

    return call_result.return_value();
}


InterpretResult interpret(ObjectStore &object_store, Environment &env, const Node &ast) {
    Interpreter interpreter(object_store, env, ast);
    return interpreter.interpret();
}
