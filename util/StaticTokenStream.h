//
// Created by roel on 6/17/22.
//


#pragma once

#include "lexer/Token.h"

#include <vector>

class StaticTokenStream : public TokenStream {
public:
    explicit StaticTokenStream(std::vector<TokenType> tokentypes);

    Token next() override;

private:
    std::vector<Token> tokens;
    std::vector<Token>::iterator it;
};


