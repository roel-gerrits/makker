//
// Created by roel on 5/24/22.
//

#include "Lexer.h"
#include "Scanner.h"
#include "Token.h"


Lexer::Lexer(Scanner &scanner_) :
        scanner(scanner_) {
}

Token Lexer::next() {

    while (true) {
        // Ignore whitespace
        if (scanner.has_more() && std::isspace(scanner.peek())) {
            scanner.next();
            continue;
        }

        // Line comment
        if (check('#')) {
            while (scanner.has_more() && scanner.peek() != '\n') {
                scanner.next();
            }
            continue;
        }

        // Block comment
        if (check('/')) {
            expect('*');

            while (true) {
                scanner.next();
                if (!check('*')) continue;
                if (!check('/')) continue;
                break;
            }
            continue;
        }

        break;
    }

    // EOS
    if (!scanner.has_more()) {
        return {scanner.get_position(), TokenType::EOS};
    }

    Position token_start_position = scanner.get_position();

    // IDENTIFIER or keyword
    if (std::isalpha(scanner.peek())) {
        std::string identifier;
        while (scanner.has_more() && (std::isalnum(scanner.peek()) || scanner.peek() == '_')) {
            identifier += scanner.next();
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
            if (!scanner.has_more()) {
                throw UnexpectedEnd(scanner.get_position());
            }

            if (!escaped && check('\\')) {
                escaped = true;
                continue;
            }

            if (!escaped && check('"')) {
                break;
            }

            escaped = false;
            str += scanner.next();
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

    throw UnexpectedCharacter(scanner.peek(), scanner.get_position());
}

bool Lexer::check(char ch) {

    if (!scanner.has_more()) {
        return false;
    }

    const bool found = scanner.peek() == ch;
    if (found) scanner.next();
    return found;
}

void Lexer::expect(char ch) {

    if (!scanner.has_more()) {
        throw UnexpectedEnd(scanner.get_position());
    }

    if (scanner.next() != ch) {
        throw UnexpectedCharacter(scanner.peek(), scanner.get_position());
    }
}
