//
// Created by roel on 7/17/22.
//

#include "Interpreter.h"
#include "ScopeWrapper.h"

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

Interpreter::Interpreter(ObjectStore &object_store_, Scope &root_scope_, const Node &ast_) :
        object_store(object_store_),
        root_scope(root_scope_),
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
    parse_function_call(root_scope, node);
}

void Interpreter::interpret_assignment_statement(const Node &node) {
    const std::string &variable = node.get_child(0).get_token().value;
    const Object &value = parse_expression(root_scope, node.get_child(1));

    if (NullObject::is_null(value)) {
        result.add_error(node, "Cannot put variable to null");
        throw InterpretError();
    }

    try {
        root_scope.put(variable, value);
    } catch (const Scope::AlreadyDefinedError &) {
        result.add_error(node, "Variable already defined");
        throw InterpretError();
    }
}

const Object &Interpreter::parse_expression(const Scope &scope, const Node &node) {

    if (node.get_type() == NodeType::OBJECT) {
        const std::string &id = node.get_token().value;

        if (!node.get_children().empty()) {
            return parse_expression(scope, node.get_child(0)).attr(id);
        }

        try {
            return scope.get(id);
        } catch (Scope::UndefinedVariableError &) {
            result.add_error(node, "Undefined variable");
            throw InterpretError();
        }
    }

    if (node.get_type() == NodeType::CALL_STATEMENT) {
        return parse_function_call(scope, node);
    }

    if (node.get_type() == NodeType::STRING) {
        const Object &obj = object_store.create_string(node.get_token().value);
        return obj;
    }

    if (node.get_type() == NodeType::LIST) {
        std::list<std::reference_wrapper<const Object>> entries;
        for (const auto &entry_node: node.get_children()) {
            entries.emplace_back(parse_expression(scope, entry_node));
        }
        return object_store.create_list(entries);
    }

    if (node.get_type() == NodeType::LIST_FOR) {
        return parse_list_for(scope, node);
    }

    result.add_error(node, "Unexpected node '" + std::string(to_str(node.get_type())) + "'");
    throw InterpretError();
}

const Object &Interpreter::parse_list_for(const Scope &scope, const Node &node) {
    const Node &expr_node = node.get_child(0);
    const Node &var_node = node.get_child(1);
    const Object &input_list = parse_expression(scope, node.get_child(2));

    std::list<std::reference_wrapper<const Object>> output_list;
    for (const Object &input_obj: input_list.entries()) {
        ScopeWrapper expr_scope(scope);
        expr_scope.put(var_node.get_token().value, input_obj);
        const Object &output_obj = parse_expression(expr_scope, expr_node);
        output_list.emplace_back(output_obj);
    }

    return object_store.create_list(output_list);
}

const Object &Interpreter::parse_function_call(const Scope &scope, const Node &node) {
    const Object &function_object = parse_expression(scope, node.get_child(0));

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
            arg_list.add(arg_node->get_token().value, arg_store.emplace_back(node, parse_expression(scope, arg_node->get_child(0))));
        } else {
            arg_list.add(arg_store.emplace_back(node, parse_expression(scope, *arg_node)));
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


InterpretResult interpret(ObjectStore &object_store, Scope &root_scope, const Node &ast) {
    Interpreter interpreter(object_store, root_scope, ast);
    return interpreter.interpret();
}
