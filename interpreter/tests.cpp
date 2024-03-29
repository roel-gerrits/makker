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

#include "parser/Parser.h"
#include "parser/DefaultParser.h"
#include "parser/StringSource.h"
#include "Interpreter.h"
#include "RootScope.h"
#include "ScopeWrapper.h"
#include "BasicObjectStore.h"
#include "parser/StaticImportResolver.h"

#include <functional>
#include <utility>

static Node parse_str_with_import(const std::string &input, ImportResolver &import_resolver) {
    DefaultParser parser(import_resolver);
    StringSource source(input);
    auto result = parser.parse(source);
    Node ast = result.ast();
    return ast;
}

static Node parse_str(const std::string &input) {
    StaticImportResolver import_resolver;
    return parse_str_with_import(input, import_resolver);
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

TEST(Scope, test_define_get) {
    BasicObjectStore store;
    RootScope scope;
    Object &x = store.create_struct({});
    scope.put("x", x);
    EXPECT_THAT(scope.get("x"), Ref(x));
}

TEST(Scope, test_scope_wrapping) {
    BasicObjectStore store;
    RootScope base_scope;
    const Object &one = store.create_string("one");
    const Object &two = store.create_string("two");
    base_scope.put("x", one);

    ScopeWrapper wrapped_scope(base_scope);
    EXPECT_THAT(wrapped_scope.get("x"), Ref(one));

    wrapped_scope.put("x", two);
    EXPECT_THAT(wrapped_scope.get("x"), Ref(two));
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

TEST(Interpreter, test_lists_of_variables) {
    BasicObjectStore store;
    RootScope scope;

    const Object &a = store.create_string("a");
    const Object &b = store.create_string("b");
    const Object &c = store.create_string("c");
    scope.put("a", a);
    scope.put("b", b);
    scope.put("c", c);

    const auto result = interpret(store, scope, parse_str("x = [a b c]"));
    const Object &x = scope.get("x");
    EXPECT_THAT(x.entries().at(0).get(), Ref(a));
    EXPECT_THAT(x.entries().at(1).get(), Ref(b));
    EXPECT_THAT(x.entries().at(2).get(), Ref(c));
}

TEST(Interpreter, test_lists_of_strings) {
    BasicObjectStore store;
    RootScope scope;

    const auto result = interpret(store, scope, parse_str("x = [\"a\" \"b\" \"c\"]"));
    const Object &x = scope.get("x");
    EXPECT_THAT(x.entries().at(0).get().get_string(), Eq("a"));
    EXPECT_THAT(x.entries().at(1).get().get_string(), Eq("b"));
    EXPECT_THAT(x.entries().at(2).get().get_string(), Eq("c"));
}

TEST(Interpreter, test_lists_for) {
    BasicObjectStore store;
    RootScope scope;

    SimpleCallHandler f_concat([&](const CallArgList &args) {
        const std::string str1 = args.arg(0).object().get_string();
        const std::string str2 = args.arg(1).object().get_string();

        return CallResult(store.create_string(str1 + str2));
    });
    scope.put("f_concat", store.create_function(f_concat));

    const auto result = interpret(store, scope, parse_str("x = [f_concat(x \"q\") for x in [\"a\" \"b\" \"c\"]]"));

    const Object &x = scope.get("x");
    EXPECT_THAT(x.entries().at(0).get().get_string(), Eq("aq"));
    EXPECT_THAT(x.entries().at(1).get().get_string(), Eq("bq"));
    EXPECT_THAT(x.entries().at(2).get().get_string(), Eq("cq"));
}

TEST(Interpreter, test_lists_for_faulty_var) {
    BasicObjectStore store;
    RootScope scope;

    SimpleCallHandler f([&](const CallArgList &args) {
        return CallResult(NullObject::get_instance());
    });
    scope.put("f", store.create_function(f));

    const auto result = interpret(store, scope, parse_str("x = [f(x) for y in [\"a\"]]"));
    EXPECT_THAT(result.success(), IsFalse());
}

TEST(Interpreter, test_import_assignment) {
    BasicObjectStore store;
    RootScope scope;
    StaticImportResolver import_resolver;
    StringSource source("t=\"txt\"");
    import_resolver.set("t.mkr", source);
    interpret(store, scope, parse_str_with_import("a=import(\"t.mkr\")", import_resolver));
    EXPECT_THAT(scope.get("a").attr("t").get_string(), Eq("txt"));
}