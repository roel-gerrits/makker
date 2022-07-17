//
// Created by roel on 6/13/22.
//


#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

#include "Parser.h"
#include "lexer/Lexer.h"
#include "lexer/StringScanner.h"
#include "util/StaticTokenStream.h"
#include "util/RewindableTokenStream.h"

TEST(RewindableTokenStream, test_next) {
    StaticTokenStream tokens({TokenType::IDENTIFIER, TokenType::STRING, TokenType::ASSIGN});
    RewindableTokenStream rts(tokens);
    EXPECT_THAT(rts.next().type, Eq(TokenType::IDENTIFIER));
    EXPECT_THAT(rts.next().type, Eq(TokenType::STRING));
    EXPECT_THAT(rts.next().type, Eq(TokenType::ASSIGN));
}

TEST(RewindableTokenStream, test_rewind_full) {
    StaticTokenStream tokens({TokenType::IDENTIFIER, TokenType::STRING, TokenType::ASSIGN});
    RewindableTokenStream rts(tokens);
    auto snapshot = rts.snapshot();
    rts.next();
    rts.next();
    rts.next();
    rts.rewind(snapshot);
    EXPECT_THAT(rts.next().type, Eq(TokenType::IDENTIFIER));
    EXPECT_THAT(rts.next().type, Eq(TokenType::STRING));
    EXPECT_THAT(rts.next().type, Eq(TokenType::ASSIGN));
}

TEST(RewindableTokenStream, test_rewind_half) {
    StaticTokenStream tokens({TokenType::IDENTIFIER, TokenType::STRING, TokenType::ASSIGN});
    RewindableTokenStream rts(tokens);
    rts.next();
    auto snapshot = rts.snapshot();
    rts.next();
    rts.next();
    rts.rewind(snapshot);
    EXPECT_THAT(rts.next().type, Eq(TokenType::STRING));
    EXPECT_THAT(rts.next().type, Eq(TokenType::ASSIGN));
}

static std::string ast_to_string(const Node &node) {
    std::string result;
    result.append(to_str(node.get_type()));
    if (!node.get_data().empty()) {
        result.append(":" + node.get_data());
    }
    if (!node.get_children().empty()) {
        result.append(" ( ");
        for (const Node &child: node.get_children()) {
            result.append(ast_to_string(child));
            result.append(" ");
        }
        result.append(")");
    }
    return result;
}

static std::string parse_str(const std::string input) {
    StringScanner s(input);
    Lexer lex(s);
    return ast_to_string(parse(lex));
}

static void print_error(const StringScanner &scanner, const UnexpectedTokenError &e) {
    printf("\nERROR \n");
    Position pos = e.get_token().pos;
    printf("%04u: %s\n", pos.line, scanner.get_line(pos).c_str());
    std::string filler(pos.col, ' ');
    printf("     %s^-- %s\n", filler.c_str(), e.what());
}

TEST(Parser, test_empty_program) {
    EXPECT_THAT(
            parse_str(""),
            Eq("PROGRAM")
    );
}

TEST(Parser, test_assignment_1) {
    EXPECT_THAT(
            parse_str("var = obj"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:var OBJECT:obj ) )")
    );
}

TEST(Parser, test_assignment_2) {
    EXPECT_THAT(
            parse_str("var = \"str\""),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:var STRING:str ) )")
    );
}

TEST(Parser, test_object) {
    EXPECT_THAT(
            parse_str("var = foo.bar.test"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:var OBJECT:test ( OBJECT:bar ( OBJECT:foo ) ) ) )")
    );
}

TEST(Parser, test_expr_parenthesis) {
    EXPECT_THAT(
            parse_str("var = (foo)"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:var OBJECT:foo ) )")
    );
}

TEST(Parser, test_call_assignment) {
    EXPECT_THAT(
            parse_str("var = func()"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:var CALL_STATEMENT ( OBJECT:func ) ) )")
    );
}

TEST(Parser, test_call_assignment_with_arg) {
    EXPECT_THAT(
            parse_str("var = func(arg1)"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:var CALL_STATEMENT ( OBJECT:func OBJECT:arg1 ) ) )")
    );
}

TEST(Parser, test_call_assignment_with_args) {
    EXPECT_THAT(
            parse_str("var = func(arg1 arg2)"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:var CALL_STATEMENT ( OBJECT:func OBJECT:arg1 OBJECT:arg2 ) ) )")
    );
}

TEST(Parser, test_call_statement) {
    EXPECT_THAT(
            parse_str("func()"),
            Eq("PROGRAM ( CALL_STATEMENT ( OBJECT:func ) )")
    );
}

TEST(Parser, test_call_statement_with_args) {
    EXPECT_THAT(
            parse_str("func(arg1 arg2)"),
            Eq("PROGRAM ( CALL_STATEMENT ( OBJECT:func OBJECT:arg1 OBJECT:arg2 ) )")
    );
}

TEST(Parser, test_call_statement_with_kwargs) {
    EXPECT_THAT(
            parse_str("func(arg1 arg2 key1=arg3 key2=arg4)"),
            Eq("PROGRAM ( CALL_STATEMENT ( OBJECT:func OBJECT:arg1 OBJECT:arg2 OBJECT:arg3 OBJECT:arg4 ) )")
    );
}

TEST(Parser, test_list) {
    EXPECT_THAT(
            parse_str("x = [ x1 x2 x3 ]"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:x LIST ( OBJECT:x1 OBJECT:x2 OBJECT:x3 ) ) )")
    );
}

TEST(Parser, test_empty_list) {
    EXPECT_THAT(
            parse_str("x = []"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:x LIST ) )")
    );
}

TEST(Parser, test_list_for) {
    EXPECT_THAT(
            parse_str("x = [ foo.bar for foo in arg ]"),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:x LIST_FOR ( OBJECT:bar ( OBJECT:foo ) VARIABLE:foo OBJECT:arg ) ) )")
    );
}