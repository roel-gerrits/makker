//
// Created by roel on 5/22/22.
//

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

#include "Lexer.h"
#include "StringSource.h"


TEST(Lexer, test_identifier) {
    StringSource s("test123");
    Lexer lex(s);
    ASSERT_THAT(lex.next().type, Eq(TokenType::IDENTIFIER));
}

TEST(Lexer, test_identifier_value) {
    StringSource s("test123");
    Lexer lex(s);
    Token t = lex.next();
    ASSERT_THAT(t.type, Eq(TokenType::IDENTIFIER));
    ASSERT_THAT(t.value, StrEq("test123"));
}

TEST(Lexer, test_invalid_identifier) {
    StringSource s("123test");
    Lexer lex(s);
    EXPECT_THROW({
                     lex.next();
                 }, UnexpectedCharacter);
}

TEST(Lexer, test_string) {
    StringSource s("\"test123\"");
    Lexer lex(s);
    ASSERT_THAT(lex.next().type, Eq(TokenType::STRING));
}

TEST(Lexer, test_string_value) {
    StringSource s("\"test123\"");
    Lexer lex(s);
    Token t = lex.next();
    ASSERT_THAT(t.type, Eq(TokenType::STRING));
    ASSERT_THAT(t.value, StrEq("test123"));
}

TEST(Lexer, test_string_empty) {
    StringSource s("\"\"");
    Lexer lex(s);
    Token t = lex.next();
    ASSERT_THAT(t.type, Eq(TokenType::STRING));
    ASSERT_THAT(t.value, StrEq(""));
}

TEST(Lexer, test_string_escape) {
    StringSource s("\"\\\"\"");
    Lexer lex(s);
    Token t = lex.next();
    ASSERT_THAT(t.type, Eq(TokenType::STRING));
    ASSERT_THAT(t.value, StrEq("\""));
}

TEST(Lexer, test_invalid_string) {
    StringSource s("\"string");
    Lexer lex(s);
    EXPECT_THROW({
                     lex.next();
                 }, UnexpectedEnd);
}

TEST(Lexer, test_for) {
    StringSource s("for");
    Lexer lex(s);
    ASSERT_THAT(lex.next().type, Eq(TokenType::FOR));
}

TEST(Lexer, test_in) {
    StringSource s("in");
    Lexer lex(s);
    ASSERT_THAT(lex.next().type, Eq(TokenType::IN));
}

TEST(Lexer, test_multiple) {
    StringSource s("identifier \"string\".[]()= for in");
    Lexer lex(s);

    ASSERT_THAT(lex.next().type, Eq(TokenType::IDENTIFIER));
    ASSERT_THAT(lex.next().type, Eq(TokenType::STRING));
    ASSERT_THAT(lex.next().type, Eq(TokenType::DOT));
    ASSERT_THAT(lex.next().type, Eq(TokenType::BRACK_OPEN));
    ASSERT_THAT(lex.next().type, Eq(TokenType::BRACK_CLOSE));
    ASSERT_THAT(lex.next().type, Eq(TokenType::PAR_OPEN));
    ASSERT_THAT(lex.next().type, Eq(TokenType::PAR_CLOSE));
    ASSERT_THAT(lex.next().type, Eq(TokenType::ASSIGN));
    ASSERT_THAT(lex.next().type, Eq(TokenType::FOR));
    ASSERT_THAT(lex.next().type, Eq(TokenType::IN));
    ASSERT_THAT(lex.next().type, Eq(TokenType::EOS));
}

TEST(Lexer, test_block_comment) {
    StringSource s("identifier1/* comment 1*1=1 */ identifier2");
    Lexer lex(s);
    ASSERT_THAT(lex.next().value, StrEq("identifier1"));
    ASSERT_THAT(lex.next().value, StrEq("identifier2"));
    ASSERT_THAT(lex.next().type, TokenType::EOS);
}

TEST(Lexer, test_line_comment) {
    StringSource s("identifier1 # comment identifier2 ");
    Lexer lex(s);
    ASSERT_THAT(lex.next().value, StrEq("identifier1"));
    ASSERT_THAT(lex.next().type, TokenType::EOS);
}

TEST(Lexer, test_multiline_comment) {
    StringSource s("identifier1 # comment1\n#comment2\n identifier2 ");
    Lexer lex(s);
    ASSERT_THAT(lex.next().value, StrEq("identifier1"));
    ASSERT_THAT(lex.next().value, StrEq("identifier2"));
    ASSERT_THAT(lex.next().type, TokenType::EOS);
}

TEST(Lexer, test_practical_example) {
    const std::string input = R"----(
# commment

/*
Block comment
*/

tools = import("tools.mkr")

compile = tools.compile
    .with_flags("Wall")

link = tools.link
    .with_flags("Wall")
    .without_flag("Xlolz")

prslib = tools.unpack(tools.mvn_get(artifacts.com_group_etc_prslib type="tar.gz"))

includes = [
    "."
    prslib.subpath("prslib/common/include")
]

exe = link([
    compile("util.cpp" include=includes)
    compile("main.cpp" include=includes)
] libs=[prslib.subpath("prslib/lib/prslib.so")])

x = [y.z for y in q]

export(exe)
)----";

    StringSource s(input);

    Lexer lex(s);

    // tools = import("tools.mkr")
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);

    //    compile = tools.compile
    //            .with_flags("Wall")
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);

    //    link = tools.link
    //            .with_flags("Wall")
    //            .without_flag("Xlolz")
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);

    //    prslib = tools.unpack(tools.mvn_get(artifacts.com_group_etc_prslib type="tar.gz"))
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);

    // includes = [
    //      "."
    //      prslib.subpath("prslib/common/include")
    // ]
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::BRACK_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);
    ASSERT_THAT(lex.next().type, TokenType::BRACK_CLOSE);

    //    exe = link([
    //        compile("util.cpp" include=includes)
    //        compile("main.cpp" include=includes)
    //    ] libs=[prslib.subpath("prslib/lib/prslib.so")])
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::BRACK_OPEN);

    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);

    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);

    ASSERT_THAT(lex.next().type, TokenType::BRACK_CLOSE);

    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::BRACK_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::STRING);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);
    ASSERT_THAT(lex.next().type, TokenType::BRACK_CLOSE);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);

    //    x = [y.z for y in q]
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::ASSIGN);
    ASSERT_THAT(lex.next().type, TokenType::BRACK_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::DOT);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::FOR);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::IN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::BRACK_CLOSE);

    //    export(exe)
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_OPEN);
    ASSERT_THAT(lex.next().type, TokenType::IDENTIFIER);
    ASSERT_THAT(lex.next().type, TokenType::PAR_CLOSE);

    //
    ASSERT_THAT(lex.next().type, TokenType::EOS);
}