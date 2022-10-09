//
// Created by roel on 5/30/22.
//

#pragma once

#include <stdexcept>

#include "lexer/Token.h"
#include "Ast.h"
#include "ImportResolver.h"
#include "util/RewindableTokenStream.h"


class Parser {
public:
    explicit Parser(ImportResolver &import_resolver);



    Node parse(TokenStream &tokens);

private:
    ImportResolver &import_resolver;

    Node parse_program(RewindableTokenStream &tokens);

    Node parse_statement(RewindableTokenStream &tokens);

    Node parse_assignment_statement(RewindableTokenStream &tokens);

    Node parse_expr(RewindableTokenStream &tokens);

    Node parse_list_for(RewindableTokenStream &tokens);

    Node parse_list(RewindableTokenStream &tokens);

    Node parse_object(RewindableTokenStream &tokens);

    Node parse_variable(RewindableTokenStream &tokens);

    Node parse_import_statement(RewindableTokenStream &tokens);

    Node parse_call_statement(RewindableTokenStream &tokens);

    Node parse_call_arg(RewindableTokenStream &tokens);

    Token read(RewindableTokenStream &tokens, TokenType type);
};


class UnexpectedTokenError : public std::runtime_error {
public:
    explicit UnexpectedTokenError(const Token &token) :
            std::runtime_error("UnexpectedToken " + std::string(to_str(token.type))),
            token(token) {}

    [[nodiscard]] const Token &get_token() const {
        return token;
    }

private:
    const Token &token;
};