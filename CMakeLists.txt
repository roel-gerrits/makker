cmake_minimum_required(VERSION 3.23)
project(mkr)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("-Wall")
add_compile_options("-Wswitch-enum")

include_directories(".")

# tests
add_executable(all_tests
        parser/tests.cpp
        interpreter/tests.cpp
        lexer/tests.cpp
        util/tests.cpp

        util/InterpretResultPrinter.cpp
        util/StaticTokenStream.cpp
        util/AstSerializer.cpp
        util/RewindableTokenStream.cpp
        lexer/Token.cpp
        lexer/Lexer.cpp
        lexer/StringScanner.cpp
        parser/Ast.cpp
        parser/Parser.cpp
        interpreter/Environment.cpp
        interpreter/Interpreter.cpp
        interpreter/BasicObjectStore.cpp
        interpreter/Object.cpp
        parser/Ast.cpp
        parser/Parser.cpp
        )
target_link_libraries(all_tests gtest gmock gtest_main)


# mkr executable
add_executable(mkr
        parser/Parser.cpp
        parser/Ast.cpp
        lexer/StringScanner.cpp
        lexer/Token.cpp
        lexer/Lexer.cpp
        util/RewindableTokenStream.cpp
        mkr/Shell.cpp
        mkr/main.cpp
        )
target_link_libraries(mkr)



