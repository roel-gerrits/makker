//
// Created by roel on 5/30/22.
//

#pragma once

#include <stdexcept>

#include "lexer/Token.h"
#include "Ast.h"


Node parse(TokenStream &tokens);


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