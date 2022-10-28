//
// Created by roel on 10/11/22.
//


#pragma once

#include "Parser.h"
#include "ast/Ast.h"
#include <stdexcept>

#include "util/RewindableTokenStream.h"

class DefaultParser : public Parser {
public:
    explicit DefaultParser(ImportResolver &import_resolver);

    Result parse(Source &source) override;

private:
    ImportResolver &import_resolver;

    Node parse_program(Result &result, RewindableTokenStream &tokens);

    Node parse_statement(Result &result, RewindableTokenStream &tokens);

    Node parse_assignment_statement(Result &result, RewindableTokenStream &tokens);

    Node parse_expr(Result &result, RewindableTokenStream &tokens);

    Node parse_list_for(Result &result, RewindableTokenStream &tokens);

    Node parse_list(Result &result, RewindableTokenStream &tokens);

    Node parse_object(Result &result, RewindableTokenStream &tokens);

    Node parse_variable(Result &result, RewindableTokenStream &tokens);

    Node parse_import_statement(Result &result, RewindableTokenStream &tokens);

    Node parse_call_statement(Result &result, RewindableTokenStream &tokens);

    Node parse_call_arg(Result &result, RewindableTokenStream &tokens);

    const Token &read(RewindableTokenStream &tokens, TokenType type);
};


class UnexpectedTokenError : public std::runtime_error {
public:
    explicit UnexpectedTokenError(const Token &token_) :
            std::runtime_error("UnexpectedToken " + std::string(to_str(token_.type))),
            token(token_) {}

    [[nodiscard]] const Token &get_token() const {
        return token;
    }

private:
    const Token &token;
};