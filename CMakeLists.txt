cmake_minimum_required(VERSION 3.23)
project(makker)

set(CMAKE_CXX_STANDARD 20)

include_directories(".")

add_executable(makker
        main.cpp
        lexer/Token.cpp
        lexer/Lexer.cpp
        lexer/Lexer.cpp
        lexer/StringScanner.cpp

        parser/Parser.cpp
        )

# lexer tests
add_executable(lexer_tests
        lexer/Token.cpp
        lexer/Lexer.cpp
        lexer/Lexer.cpp
        lexer/StringScanner.cpp
        lexer/tests.cpp
        )
target_link_libraries(lexer_tests gtest gmock gtest_main)

# parser tests
add_executable(parser_tests
        util/StaticTokenStream.cpp
        util/AstSerializer.cpp
        util/RewindableTokenStream.cpp
        lexer/Token.cpp
        lexer/Lexer.cpp
        lexer/StringScanner.cpp
        parser/Ast.cpp
        parser/Parser.cpp
        parser/Traverser.cpp
        parser/tests.cpp
        )
target_link_libraries(parser_tests gtest gmock gtest_main)


