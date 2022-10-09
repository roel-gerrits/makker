//
// Created by roel on 6/13/22.
//


#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

#include "Parser.h"
#include "lexer/Lexer.h"
#include "lexer/StringScanner.h"
#include "StaticImportResolver.h"

static std::string ast_to_string(const Node &node) {
    std::string result;
    result.append(to_str(node.get_type()));
    if (!node.get_token().value.empty()) {
        result.append(":" + node.get_token().value);
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

static Node parse(const std::string &input) {
    StaticImportResolver import_resolver;
    Parser parser(import_resolver);
    StringScanner s(input);
    Lexer lex(s);
    return parser.parse(lex);
}

static std::string parse_str(const std::string &input, ImportResolver &import_resolver) {
    Parser parser(import_resolver);
    StringScanner s(input);
    Lexer lex(s);
    return ast_to_string(parser.parse(lex));
}

static std::string parse_str(const std::string &input) {
    StaticImportResolver import_resolver;
    return parse_str(input, import_resolver);
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
            Eq("PROGRAM:var ( ASSIGNMENT_STATEMENT:var ( VARIABLE:var OBJECT:obj ) )")
    );
}

TEST(Parser, test_assignment_2) {
    EXPECT_THAT(
            parse_str("var = \"str\""),
            Eq("PROGRAM:var ( ASSIGNMENT_STATEMENT:var ( VARIABLE:var STRING:str ) )")
    );
}

TEST(Parser, test_object) {
    EXPECT_THAT(
            parse_str("var = foo.bar.test"),
            Eq("PROGRAM:var ( ASSIGNMENT_STATEMENT:var ( VARIABLE:var OBJECT:test ( OBJECT:bar ( OBJECT:foo ) ) ) )")
    );
}

TEST(Parser, test_expr_parenthesis) {
    EXPECT_THAT(
            parse_str("var = (foo)"),
            Eq("PROGRAM:var ( ASSIGNMENT_STATEMENT:var ( VARIABLE:var OBJECT:foo ) )")
    );
}

TEST(Parser, test_call_assignment) {
    EXPECT_THAT(
            parse_str("var = func()"),
            Eq("PROGRAM:var ( ASSIGNMENT_STATEMENT:var ( VARIABLE:var CALL_STATEMENT:func ( OBJECT:func ) ) )")
    );
}

TEST(Parser, test_call_assignment_with_arg) {
    EXPECT_THAT(
            parse_str("var = func(arg1)"),
            Eq("PROGRAM:var ( ASSIGNMENT_STATEMENT:var ( VARIABLE:var CALL_STATEMENT:func ( OBJECT:func OBJECT:arg1 ) ) )")
    );
}

TEST(Parser, test_call_assignment_with_args) {
    EXPECT_THAT(
            parse_str("var = func(arg1 arg2)"),
            Eq("PROGRAM:var ( ASSIGNMENT_STATEMENT:var ( VARIABLE:var CALL_STATEMENT:func ( OBJECT:func OBJECT:arg1 OBJECT:arg2 ) ) )")
    );
}

TEST(Parser, test_call_statement) {
    EXPECT_THAT(
            parse_str("func()"),
            Eq("PROGRAM:func ( CALL_STATEMENT:func ( OBJECT:func ) )")
    );
}

TEST(Parser, test_call_statement_with_args) {
    EXPECT_THAT(
            parse_str("func(arg1 arg2)"),
            Eq("PROGRAM:func ( CALL_STATEMENT:func ( OBJECT:func OBJECT:arg1 OBJECT:arg2 ) )")
    );
}

TEST(Parser, test_call_statement_with_kwargs) {
    EXPECT_THAT(
            parse_str("func(arg1 arg2 key1=arg3 key2=arg4)"),
            Eq("PROGRAM:func ( CALL_STATEMENT:func ( OBJECT:func OBJECT:arg1 OBJECT:arg2 KWARG:key1 ( OBJECT:arg3 ) KWARG:key2 ( OBJECT:arg4 ) ) )")
    );
}
TEST(Parser, test_list) {
    EXPECT_THAT(
            parse_str("x = [ x1 x2 x3 ]"),
            Eq("PROGRAM:x ( ASSIGNMENT_STATEMENT:x ( VARIABLE:x LIST ( OBJECT:x1 OBJECT:x2 OBJECT:x3 ) ) )")
    );
}

TEST(Parser, test_empty_list) {
    EXPECT_THAT(
            parse_str("x = []"),
            Eq("PROGRAM:x ( ASSIGNMENT_STATEMENT:x ( VARIABLE:x LIST ) )")
    );
}

TEST(Parser, test_list_for) {
    EXPECT_THAT(
            parse_str("x = [ foo.bar for foo in arg ]"),
            Eq("PROGRAM:x ( ASSIGNMENT_STATEMENT:x ( VARIABLE:x LIST_FOR ( OBJECT:bar ( OBJECT:foo ) VARIABLE:foo OBJECT:arg ) ) )")
    );
}

TEST(Parser, test_import_statement) {
    StaticImportResolver import_resolver;
    StringScanner s("a=\"a\"");
    Lexer lex(s);
    import_resolver.set("file.mkr", lex);
    EXPECT_THAT(
            parse_str("x = import(\"file.mkr\")", import_resolver),
            Eq("PROGRAM:x ( ASSIGNMENT_STATEMENT:x ( VARIABLE:x PROGRAM:a ( ASSIGNMENT_STATEMENT:a ( VARIABLE:a STRING:a ) ) ) )")
    );
}

TEST(Parser, test_import_failure) {
    StaticImportResolver import_resolver;
    StringScanner s("a=\"a\"");
    Lexer lex(s);
    import_resolver.set("file.mkr", lex);
    EXPECT_THROW({
                     parse_str("x = import(\"unknown-file.mkr\")", import_resolver);

                 }, ImportError);
}
