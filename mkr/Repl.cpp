//
// Created by roel on 10/1/22.
//

#include "Repl.h"

#include "parser/DefaultParser.h"


Repl::Repl(ImportResolver &import_resolver_, ObjectStore &object_store_, Scope &root_scope_) :
        import_resolver(import_resolver_),
        object_store(object_store_),
        root_scope(root_scope_) {}

Repl::EvalResult Repl::eval(Source &source) {
    DefaultParser parser(import_resolver);
    Parser::Result parse_result = parser.parse(source);

    EvalResult eval_result;
    for (const Parser::Result::Error &error: parse_result.errors()) {
        eval_result.add_error(error.source_location, error.message);
    }

    if (!parse_result.success()) return eval_result;

    Node ast = parse_result.ast();
    Interpreter interpreter(object_store, root_scope, ast);
    InterpretResult interpret_result = interpreter.interpret();

    for (const InterpretResult::Error &error: interpret_result.errors()) {
        eval_result.add_error(error.node.get_source_location(), error.message);
    }

    return eval_result;
}

