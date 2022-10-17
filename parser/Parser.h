//
// Created by roel on 10/11/22.
//


#pragma once

#include "Source.h"
#include <string>
#include <list>

#include "ImportResolver.h"
#include "Source.h"
#include "ast/Ast.h"

class Parser {
public:

    class Result {
    public:

        [[nodiscard]] bool success() const { return error_list.empty(); }

        class Error {
        public:
            const Source::Location &source_location;
            const std::string message;
        };

        void add_error(const Source::Location &source_location, const std::string &msg) {
            error_list.push_back({source_location, msg});
        }

        [[nodiscard]] const std::list<Error> &errors() const { return error_list; }

        const Node &ast() { return _ast.value(); }

        void set_ast(Node ast) { _ast.emplace(std::move(ast)); }

    private:
        std::list<Error> error_list;
        std::optional<Node> _ast;
    };

    virtual Result parse(Source &source) = 0;
};


