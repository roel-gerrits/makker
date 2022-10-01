//
// Created by roel on 9/20/22.
//


#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;


#include "util/StaticTokenStream.h"
#include "util/RewindableTokenStream.h"

TEST(RewindableTokenStream, test_next) {
    StaticTokenStream tokens({TokenType::IDENTIFIER, TokenType::STRING, TokenType::ASSIGN});
    RewindableTokenStream rts(tokens);
    EXPECT_THAT(rts.next().type, Eq(TokenType::IDENTIFIER));
    EXPECT_THAT(rts.next().type, Eq(TokenType::STRING));
    EXPECT_THAT(rts.next().type, Eq(TokenType::ASSIGN));
}

TEST(RewindableTokenStream, test_next_peek) {
    StaticTokenStream tokens({TokenType::IDENTIFIER, TokenType::STRING, TokenType::ASSIGN});
    RewindableTokenStream rts(tokens);
    EXPECT_THAT(rts.peek().type, Eq(TokenType::IDENTIFIER));
    rts.next();

    EXPECT_THAT(rts.peek().type, Eq(TokenType::STRING));
    rts.next();

    EXPECT_THAT(rts.peek().type, Eq(TokenType::ASSIGN));
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

TEST(RewindableTokenStream, test_rewind_peek) {
    StaticTokenStream tokens({TokenType::IDENTIFIER, TokenType::STRING, TokenType::ASSIGN});
    RewindableTokenStream rts(tokens);
    auto snapshot = rts.snapshot();
    rts.next();
    rts.next();
    rts.next();
    rts.rewind(snapshot);
    EXPECT_THAT(rts.peek().type, Eq(TokenType::IDENTIFIER));
    rts.next();
    EXPECT_THAT(rts.peek().type, Eq(TokenType::STRING));
    rts.next();
    EXPECT_THAT(rts.peek().type, Eq(TokenType::ASSIGN));
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

TEST(RewindableTokenStream, test_rewind_half_peek) {
    StaticTokenStream tokens({TokenType::IDENTIFIER, TokenType::STRING, TokenType::ASSIGN});
    RewindableTokenStream rts(tokens);
    rts.next();
    auto snapshot = rts.snapshot();
    rts.next();
    rts.next();
    rts.rewind(snapshot);
    EXPECT_THAT(rts.peek().type, Eq(TokenType::STRING));
    rts.next();
    EXPECT_THAT(rts.peek().type, Eq(TokenType::ASSIGN));
}
