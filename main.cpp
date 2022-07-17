#include <cstdio>

#include "lexer/old/Scanner.h"
#include "lexer/old/Lexer.h"

void print_position(const Scanner &scanner, const Position position) {
    const std::string line = scanner.get_line(position);
    std::string arrow_str = std::string(position.col - 1,' ') + '^';
    printf("%4u: %s\n", position.line, line.c_str());
    printf("      %s\n", arrow_str.c_str());
}

int main() {
    StringScanner s("test123");

    Lexer lex;
    try {
        lex.next(s);
    } catch (const UnexpectedCharacter &e) {
        printf("%s\n", e.what());
        print_position(s, e.get_position());

    }

    return 0;
}
