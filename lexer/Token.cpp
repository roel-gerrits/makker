//
// Created by roel on 6/17/22.
//

#include "Token.h"


const char *to_str(TokenType t) {
    switch (t) {
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::STRING:
            return "STRING";
        case TokenType::DOT:
            return "DOT";
        case TokenType::BRACK_OPEN:
            return "BRACK_OPEN";
        case TokenType::BRACK_CLOSE:
            return "BRACK_CLOSE";
        case TokenType::PAR_OPEN:
            return "PAR_OPEN";
        case TokenType::PAR_CLOSE:
            return "PAR_CLOSE";
        case TokenType::ASSIGN:
            return "ASSIGN";
        case TokenType::FOR:
            return "FOR";
        case TokenType::IN:
            return "IN";
        case TokenType::EOS:
            return "EOS";
        default:
            return "???";
    }
}

std::ostream &operator<<(std::ostream &os, const TokenType &t) {
    return os << std::string(to_str(t));
}