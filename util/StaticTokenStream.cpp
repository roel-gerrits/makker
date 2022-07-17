//
// Created by roel on 6/17/22.
//

#include "StaticTokenStream.h"

#include <utility>

static std::vector<Token> create_tokens(std::vector<TokenType> tokentypes) {
    std::vector<Token> tokens;
    unsigned int i = 0;
    for (const TokenType &type: tokentypes) {
        tokens.push_back(Token({i, i}, type));
    }
    return tokens;
}

StaticTokenStream::StaticTokenStream(std::vector<TokenType> tokentypes) :
        tokens(create_tokens(std::move(tokentypes))),
        it(tokens.begin()) {}

Token StaticTokenStream::next() {
    Token t = *it;
    it++;
    return t;
}
