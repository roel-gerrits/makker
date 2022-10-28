//
// Created by roel on 5/24/22.
//

#include "Lexer.h"
#include "Source.h"
#include "Token.h"


Lexer::Lexer(Source &source_) :
        source(source_) {
}

Token Lexer::next() {

    while (true) {
        // Ignore whitespace
        if (source.has_more() && std::isspace(source.peek())) {
            source.next();
            continue;
        }

        // Line comment
        if (check('#')) {
            while (source.has_more() && source.peek() != '\n') {
                source.next();
            }
            continue;
        }

        // Block comment
        if (check('/')) {
            expect('*');

            while (source.has_more()) {
                source.next();
                if (!check('*')) continue;
                if (!check('/')) continue;
                break;
            }
            continue;
        }

        break;
    }

    // EOS
    if (!source.has_more()) {
        return {source.get_location(), TokenType::EOS};
    }

    const Source::Location &token_start_position = source.get_location();

    // IDENTIFIER or keyword
    if (std::isalpha(source.peek())) {
        std::string identifier;
        while (source.has_more() && (std::isalnum(source.peek()) || source.peek() == '_')) {
            identifier += source.next();
        }

        if (identifier == "for") {
            return {token_start_position, TokenType::FOR, identifier};
        }

        if (identifier == "in") {
            return {token_start_position, TokenType::IN, identifier};
        }

        return {token_start_position, TokenType::IDENTIFIER, identifier};
    }

    // STRING
    if (check('"')) {
        std::string str;
        bool escaped = false;
        while (true) {
            if (!source.has_more()) {
                throw UnexpectedEnd(source.get_location());
            }

            if (!escaped && check('\\')) {
                escaped = true;
                continue;
            }

            if (!escaped && check('"')) {
                break;
            }

            escaped = false;
            str += source.next();
        }

        return {token_start_position, TokenType::STRING, str};
    }

    // DOT
    if (check('.')) {
        return {token_start_position, TokenType::DOT};
    }

    // BRACK_OPEN
    if (check('[')) {
        return {token_start_position, TokenType::BRACK_OPEN};
    }

    // BRACK_CLOSE
    if (check(']')) {
        return {token_start_position, TokenType::BRACK_CLOSE};
    }

    // PAR_OPEN
    if (check('(')) {
        return {token_start_position, TokenType::PAR_OPEN};
    }

    // PAR_CLOSE
    if (check(')')) {
        return {token_start_position, TokenType::PAR_CLOSE};
    }

    // ASSIGN
    if (check('=')) {
        return {token_start_position, TokenType::ASSIGN};
    }

    throw UnexpectedCharacter(source.peek(), source.get_location());
}

bool Lexer::check(char ch) {

    if (!source.has_more()) {
        return false;
    }

    const bool found = source.peek() == ch;
    if (found) source.next();
    return found;
}

void Lexer::expect(char ch) {

    if (!source.has_more()) {
        throw UnexpectedEnd(source.get_location());
    }

    if (source.next() != ch) {
        throw UnexpectedCharacter(source.peek(), source.get_location());
    }
}
