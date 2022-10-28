//
// Created by roel on 10/11/22.
//

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;
using testing::IsFalse;

#include "StringSource.h"
#include "DefaultParser.h"
#include "StaticImportResolver.h"

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

static void print_error(const Parser::Result &parse_result) {
    if (parse_result.success()) return;
    printf("\nERROR \n");
    for (const Parser::Result::Error &error: parse_result.errors()) {
        printf(" > %s\n", error.message.c_str());
        printf("     %s\n", error.source_location.annotate("").c_str());
    }
}

static Parser::Result parse_with_import(const std::string &src, ImportResolver &import_resolver) {
    DefaultParser parser(import_resolver);
    StringSource source(src);
    return parser.parse(source);
}

static std::string parse_str_with_import(const std::string &src, ImportResolver &import_resolver) {
    auto result = parse_with_import(src, import_resolver);
    print_error(result);
    return ast_to_string(result.ast());
}

static std::string parse_str(const std::string &src) {
    StaticImportResolver import_resolver;
    return parse_str_with_import(src, import_resolver);
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
            Eq("PROGRAM ( CALL_STATEMENT ( OBJECT:func OBJECT:arg1 OBJECT:arg2 KWARG:key1 ( OBJECT:arg3 ) KWARG:key2 ( OBJECT:arg4 ) ) )")
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

TEST(Parser, test_import_statement) {
    StaticImportResolver import_resolver;
    StringSource s("a=\"a\"");
    import_resolver.set("file.mkr", s);
    EXPECT_THAT(
            parse_str_with_import("x = import(\"file.mkr\")", import_resolver),
            Eq("PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:x PROGRAM ( ASSIGNMENT_STATEMENT ( VARIABLE:a STRING:a ) ) ) )")
    );
}

TEST(Parser, test_import_failure) {
    StaticImportResolver import_resolver;
    auto result = parse_with_import("x = import(\"unknown-file.mkr\")", import_resolver);
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Parser, test_parse_error) {
    StaticImportResolver import_resolver;
    auto result = parse_with_import("x", import_resolver);
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Parser, test_parse_error_2) {
    StaticImportResolver import_resolver;
    auto result = parse_with_import("4", import_resolver);
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Parser, test_parse_error_3) {
    StaticImportResolver import_resolver;
    auto result = parse_with_import("x=\"x", import_resolver);
    EXPECT_THAT(result.success(), IsFalse());
}
