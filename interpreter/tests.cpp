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

TEST(Environment, test_define_get) {
    BasicObjectStore store;
    Environment env;
    Object &x = store.create_struct({});
    env.define("x", x);
    EXPECT_THAT(env.get("x"), Ref(x));
}

TEST(Interpreter, test_assignment) {
    BasicObjectStore store;
    Environment env;
    Object &x = store.create_struct({});
    env.define("x", x);
    interpret(store, env, parse_str("a=x"));
    EXPECT_THAT(env.get("a"), Ref(x));
}

TEST(Interpreter, test_undefined_error) {
    BasicObjectStore store;
    Environment env;
    auto result = interpret(store, env, parse_str("a=x"));
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_already_defined_error) {
    BasicObjectStore store;
    Environment env;
    env.define("a", store.create_struct({}));
    env.define("b", store.create_struct({}));
    auto result = interpret(store, env, parse_str("a=b"));
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_multi_assignment) {
    BasicObjectStore store;
    Environment env;
    Object &x = store.create_struct({});
    Object &y = store.create_struct({});
    env.define("x", x);
    env.define("y", y);
    interpret(store, env, parse_str("a=x b=y"));
    EXPECT_THAT(env.get("a"), Ref(x));
    EXPECT_THAT(env.get("b"), Ref(y));
}

TEST(Interpreter, test_null_assignment) {
    BasicObjectStore store;
    Environment env;
    env.define("null", NullObject::get_instance());
    auto result = interpret(store, env, parse_str("x = null"));
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_attr) {
    BasicObjectStore store;
    Environment env;
    Object &y = store.create_struct({});
    Object &x = store.create_struct({{"y", y}});
    env.define("x", x);
    interpret(store, env, parse_str("a=x.y"));
    EXPECT_THAT(env.get("a"), Ref(y));
}

TEST(Interpreter, test_attr2) {
    BasicObjectStore store;
    Environment env;
    Object &z = store.create_struct({});
    Object &y = store.create_struct({{"z", z}});
    Object &x = store.create_struct({{"y", y}});
    env.define("x", x);
    interpret(store, env, parse_str("a=x.y.z"));
    EXPECT_THAT(env.get("a"), Ref(z));
}

TEST(Interpreter, test_function_call_success) {
    BasicObjectStore store;
    Environment env;
    Object &r = store.create_struct({});

    SimpleCallHandler call_handler([&](const CallArgList &args) {
        return CallResult(r);
    });
    env.define("f", store.create_function(call_handler));

    interpret(store, env, parse_str("x = f()"));

    EXPECT_THAT(env.get("x"), Ref(r));
}

TEST(Interpreter, test_function_call_error) {
    BasicObjectStore store;
    Environment env;

    SimpleCallHandler call_handler([](const CallArgList &args) {
        CallResult result;
        result.add_call_error("error");
        return result;
    });
    env.define("f", store.create_function(call_handler));

    const auto result = interpret(store, env, parse_str("x = f()"));

    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_function_call_w_single_arg) {
    BasicObjectStore store;
    Environment env;
    Object &a = store.create_struct({});
    env.define("a", a);

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg(0).object());
    });
    env.define("f", store.create_function(call_handler));

    const auto result = interpret(store, env, parse_str("x = f(a)"));
    EXPECT_THAT(env.get("x"), Ref(a));
}

TEST(Interpreter, test_function_call_w_multiple_arg) {
    BasicObjectStore store;
    Environment env;
    Object &a = store.create_struct({});
    Object &b = store.create_struct({});
    Object &c = store.create_struct({});
    env.define("a", a);
    env.define("b", b);
    env.define("c", c);

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg(2).object());
    });

    env.define("f", store.create_function(call_handler));

    const auto result = interpret(store, env, parse_str("x = f(a b c)"));
    EXPECT_THAT(env.get("x"), Ref(c));
}

TEST(Interpreter, test_function_call_w_kwarg) {
    BasicObjectStore store;
    Environment env;
    Object &c = store.create_struct({});
    env.define("b", store.create_struct({}));
    env.define("a", store.create_struct({}));
    env.define("c", c);

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg("kw2").object());
    });
    env.define("f", store.create_function(call_handler));

    const auto result = interpret(store, env, parse_str("x = f(a kw1=b kw2=c)"));
    print_interpret_result(result);
    EXPECT_THAT(env.get("x"), Ref(c));
}

TEST(Interpreter, test_function_call_w_attr_arg) {
    BasicObjectStore store;
    Environment env;
    Object &b = store.create_struct({});
    env.define("b", b);
    env.define("a", store.create_struct({{"b", b}}));

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg(0).object());
    });
    env.define("f", store.create_function(call_handler));

    const auto result = interpret(store, env, parse_str("x = f(a.b)"));
    EXPECT_THAT(env.get("x"), Ref(b));
}

TEST(Interpreter, test_string_literal) {
    BasicObjectStore store;
    Environment env;

    const auto result = interpret(store, env, parse_str("x = \"abcd\""));
    EXPECT_THAT(env.get("x").get_string(), Eq("abcd"));
}

TEST(Interpreter, test_string_arg) {
    BasicObjectStore store;
    Environment env;

    SimpleCallHandler call_handler([](const CallArgList &args) {
        return CallResult(args.arg(0).object());
    });
    env.define("f", store.create_function(call_handler));

    const auto result = interpret(store, env, parse_str("x = f(\"abcd\")"));
    EXPECT_THAT(env.get("x").get_string(), Eq("abcd"));
}
