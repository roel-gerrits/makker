//
// Created by roel on 6/17/22.
//

#include "StaticTokenStream.h"
#include "parser/StringSource.h"
#include <utility>


static StringSource dummy_source("");

static std::vector<Token> create_tokens(std::vector<TokenType> tokentypes) {
    std::vector<Token> tokens;
    for (const TokenType &type: tokentypes) {
        tokens.emplace_back(dummy_source.get_location(), type);
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
