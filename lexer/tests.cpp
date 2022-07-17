//
// Created by roel on 5/22/22.
//

#include "gmock/gmock.h"
#include "gtest/gtest.h"
using namespace testing;

#include "Lexer.h"
#include "StringScanner.h"


TEST(Lexer, test_identifier) {
    StringScanner s("test123");
    Lexer lex(s);
    ASSERT_THAT(lex.next().type, Eq(TokenType::IDENTIFIER));
}

TEST(Lexer, test_identifier_value) {
    StringScanner s("test123");
    Lexer lex(s);
    Token t = lex.next();
    ASSERT_THAT(t.type, Eq(TokenType::IDENTIFIER));
    ASSERT_THAT(t.value, StrEq("test123"));
}

TEST(Lexer, test_invalid_identifier) {
    StringScanner s("123test");
    Lexer lex(s);
    EXPECT_THROW({
                     lex.next();
                 }, UnexpectedCharacter);
}

TEST(Lexer, test_string) {
    StringScanner s("\"test123\"");
    Lexer lex(s);
    ASSERT_THAT(lex.next().type, Eq(TokenType::STRING));
}

TEST(Lexer, test_string_value) {
    StringScanner s("\"test123\"");
    Lexer lex(s);
    Token t = lex.next();
    ASSERT_THAT(t.type, Eq(TokenType::STRING));
    ASSERT_THAT(t.value, StrEq("test123"));
}

TEST(Lexer, test_string_empty) {
    StringScanner s("\"\"");
    Lexer lex(s);
    Token t = lex.next();
    ASSERT_THAT(t.type, Eq(TokenType::STRING));
    ASSERT_THAT(t.value, StrEq(""));
}

TEST(Lexer, test_string_escape) {
    StringScanner s("\"\\\"\"");
    Lexer lex(s);
    Token t = lex.next();
    ASSERT_THAT(t.type, Eq(TokenType::STRING));
    ASSERT_THAT(t.value, StrEq("\""));
}

TEST(Lexer, test_invalid_string) {
    StringScanner s("\"string");
    Lexer lex(s);
    EXPECT_THROW({
                     lex.next();
                 }, UnexpectedEnd);
}

TEST(Lexer, test_for) {
    StringScanner s("for");
    Lexer lex(s);
    ASSERT_THAT(lex.next().type, Eq(TokenType::FOR));
}

TEST(Lexer, test_in) {
    StringScanner s("in");
    Lexer lex(s);
    ASSERT_THAT(lex.next().type, Eq(TokenType::IN));
}

TEST(Lexer, test_multiple) {
    StringScanner s("identifier \"string\".[]()= for in");
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
    StringScanner s("identifier1/* comment 1*1=1 */ identifier2");
    Lexer lex(s);
    ASSERT_THAT(lex.next().value, StrEq("identifier1"));
    ASSERT_THAT(lex.next().value, StrEq("identifier2"));
    ASSERT_THAT(lex.next().type, TokenType::EOS);
}

TEST(Lexer, test_line_comment) {
    StringScanner s("identifier1 # comment identifier2 ");
    Lexer lex(s);
    ASSERT_THAT(lex.next().value, StrEq("identifier1"));
    ASSERT_THAT(lex.next().type, TokenType::EOS);
}

TEST(Lexer, test_multiline_comment) {
    StringScanner s("identifier1 # comment1\n#comment2\n identifier2 ");
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

    StringScanner s(input);

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