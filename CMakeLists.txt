cmake_minimum_required(VERSION 3.23)
project(mkr)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("-Wall")
add_compile_options("-Wswitch-enum")

include_directories(".")

# tests
add_executable(all_tests
        parser/test_parser.cpp
        parser/test_lexer.cpp
        interpreter/tests.cpp
        util/tests.cpp

        util/StaticTokenStream.cpp
        util/RewindableTokenStream.cpp
        parser/DefaultParser.cpp
        parser/StringSource.cpp
        parser/Token.cpp
        parser/Lexer.cpp
        parser/StaticImportResolver.cpp

        ast/Ast.cpp

        interpreter/Interpreter.cpp
        interpreter/BasicObjectStore.cpp
        interpreter/Object.cpp
        interpreter/RootScope.cpp
        interpreter/ScopeWrapper.cpp
        )
target_link_libraries(all_tests gtest gmock gtest_main)


# mkr executable
add_executable(mkr
        util/RewindableTokenStream.cpp
        mkr/Shell.cpp
        mkr/main.cpp
        )
target_link_libraries(mkr)



