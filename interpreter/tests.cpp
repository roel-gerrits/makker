//
// Created by roel on 7/17/22.
//

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Ref;
using testing::IsFalse;
using testing::IsTrue;
using testing::Return;
using testing::Eq;

#include "lexer/StringScanner.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "Interpreter.h"
#include "RootScope.h"
#include "BasicObjectStore.h"
#include "util/AstSerializer.h"
#include "util/InterpretResultPrinter.h"

#include <functional>
#include <utility>

static Node parse_str(const std::string &input) {
    StringScanner s(input);
    Lexer lex(s);
    Node ast = parse(lex);
//    printf("%s \n", ast_to_string(ast).c_str());
    return ast;
}


class SimpleCallHandler : public CallHandler {
public:
    explicit SimpleCallHandler(std::function<CallResult(const CallArgList &)> handler_) :
            handler(std::move(handler_)) {}

    [[nodiscard]] CallResult call(const CallArgList &arguments) const override {
        return handler(arguments);
    }

    bool operator==(const CallHandler &rhs) const override {
        return false;
    }

private:
    std::function<CallResult(const CallArgList &)> handler;
};

TEST(RootScope, test_define_get) {
    BasicObjectStore store;
    RootScope scope;
    Object &x = store.create_struct({});
    scope.put("x", x);
    EXPECT_THAT(scope.get("x"), Ref(x));
}

TEST(Interpreter, test_assignment) {
    BasicObjectStore store;
    RootScope scope;
    Object &x = store.create_struct({});
    scope.put("x", x);
    interpret(store, scope, parse_str("a=x"));
    EXPECT_THAT(scope.get("a"), Ref(x));
}

TEST(Interpreter, test_undefined_error) {
    BasicObjectStore store;
    RootScope scope;
    auto result = interpret(store, scope, parse_str("a=x"));
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_already_defined_error) {
    BasicObjectStore store;
    RootScope scope;
    scope.put("a", store.create_struct({}));
    scope.put("b", store.create_struct({}));
    auto result = interpret(store, scope, parse_str("a=b"));
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_multi_assignment) {
    BasicObjectStore store;
    RootScope scope;
    Object &x = store.create_struct({});
    Object &y = store.create_struct({});
    scope.put("x", x);
    scope.put("y", y);
    interpret(store, scope, parse_str("a=x b=y"));
    EXPECT_THAT(scope.get("a"), Ref(x));
    EXPECT_THAT(scope.get("b"), Ref(y));
}

TEST(Interpreter, test_null_assignment) {
    BasicObjectStore store;
    RootScope scope;
    scope.put("null", NullObject::get_instance());
    auto result = interpret(store, scope, parse_str("x = null"));
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_attr) {
    BasicObjectStore store;
    RootScope scope;
    Object &y = store.create_struct({});
    Object &x = store.create_struct({{"y", y}});
    scope.put("x", x);
    interpret(store, scope, parse_str("a=x.y"));
    EXPECT_THAT(scope.get("a"), Ref(y));
}

TEST(Interpreter, test_attr2) {
    BasicObjectStore store;
    RootScope scope;
    Object &z = store.create_struct({});
    Object &y = store.create_struct({{"z", z}});
    Object &x = store.create_struct({{"y", y}});
    scope.put("x", x);
    interpret(store, scope, parse_str("a=x.y.z"));
    EXPECT_THAT(scope.get("a"), Ref(z));
}

TEST(Interpreter, test_function_call_success) {
    BasicObjectStore store;
    RootScope scope;
    Object &r = store.create_struct({});

    SimpleCallHandler call_handler([&](const CallArgList &args) {
        return CallResult(r);
    });
    scope.put("f", store.create_function(call_handler));

    interpret(store, scope, parse_str("x = f()"));

    EXPECT_THAT(scope.get("x"), Ref(r));
}

TEST(Interpreter, test_function_call_error) {
    BasicObjectStore store;
    RootScope scope;

    SimpleCallHandler call_handler([](const CallArgList &args) {
        CallResult result;
        result.add_call_error("error");
        return result;
    });
    scope.put("f", store.create_function(call_handler));

    const auto result = interpret(store, scope, parse_str("x = f()"));

    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_function_call_w_single_arg) {
    BasicObjectStore store;
    RootScope scope;
    Object &a = store.create_struct({});
    scope.put("a", a);

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg(0).object());
    });
    scope.put("f", store.create_function(call_handler));

    const auto result = interpret(store, scope, parse_str("x = f(a)"));
    EXPECT_THAT(scope.get("x"), Ref(a));
}

TEST(Interpreter, test_function_call_w_multiple_arg) {
    BasicObjectStore store;
    RootScope scope;
    Object &a = store.create_struct({});
    Object &b = store.create_struct({});
    Object &c = store.create_struct({});
    scope.put("a", a);
    scope.put("b", b);
    scope.put("c", c);

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg(2).object());
    });

    scope.put("f", store.create_function(call_handler));

    const auto result = interpret(store, scope, parse_str("x = f(a b c)"));
    EXPECT_THAT(scope.get("x"), Ref(c));
}

TEST(Interpreter, test_function_call_w_kwarg) {
    BasicObjectStore store;
    RootScope scope;
    Object &c = store.create_struct({});
    scope.put("b", store.create_struct({}));
    scope.put("a", store.create_struct({}));
    scope.put("c", c);

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg("kw2").object());
    });
    scope.put("f", store.create_function(call_handler));

    const auto result = interpret(store, scope, parse_str("x = f(a kw1=b kw2=c)"));
    print_interpret_result(result);
    EXPECT_THAT(scope.get("x"), Ref(c));
}

TEST(Interpreter, test_function_call_w_attr_arg) {
    BasicObjectStore store;
    RootScope scope;
    Object &b = store.create_struct({});
    scope.put("b", b);
    scope.put("a", store.create_struct({{"b", b}}));

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg(0).object());
    });
    scope.put("f", store.create_function(call_handler));

    const auto result = interpret(store, scope, parse_str("x = f(a.b)"));
    EXPECT_THAT(scope.get("x"), Ref(b));
}

TEST(Interpreter, test_string_literal) {
    BasicObjectStore store;
    RootScope scope;

    const auto result = interpret(store, scope, parse_str("x = \"abcd\""));
    EXPECT_THAT(scope.get("x").get_string(), Eq("abcd"));
}

TEST(Interpreter, test_string_arg) {
    BasicObjectStore store;
    RootScope scope;

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg(0).object());
    });
    scope.put("f", store.create_function(call_handler));

    const auto result = interpret(store, scope, parse_str("x = f(\"abcd\")"));
    EXPECT_THAT(scope.get("x").get_string(), Eq("abcd"));
}
