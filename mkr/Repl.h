//
// Created by roel on 10/1/22.
//


#pragma once

#include <string>
#include <utility>

#include "interpreter/BasicObjectStore.h"
#include "interpreter/RootScope.h"
#include "interpreter/Interpreter.h"
#include "parser/ImportResolver.h"

class Repl {
public:
    explicit Repl(ImportResolver &import_resolver_, ObjectStore &object_store_, Scope &root_scope_);

    class EvalResult {
    public:
        struct Error {
            const Source::Location &source_location;
            const std::string msg;
        };

        [[nodiscard]] bool success() const { return error_list.empty(); }

        void add_error(const Source::Location &source_location, std::string msg) {
            error_list.push_back({source_location, std::move(msg)});
        }

        [[nodiscard]] const std::list<Error> &errors() const { return error_list; }

    private:
        std::list<Error> error_list;
    };

    EvalResult eval(Source &source);

private:
    ImportResolver &import_resolver;
    ObjectStore &object_store;
    Scope &root_scope;
};


