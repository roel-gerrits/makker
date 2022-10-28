//
// Created by roel on 9/28/22.
//


#pragma once

#include <string>
#include <list>
#include <utility>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <optional>

class Object;


class CallArg {
public:
    [[nodiscard]] virtual const Object &object() const = 0;
};

class CallArgList {
public:
    CallArgList();

    void add(const CallArg &arg);

    void add(std::string keyword, const CallArg &arg);

    const CallArg &arg(unsigned int index) const;

    const CallArg &arg(const std::string &keyword) const;

private:
    std::list<std::reference_wrapper<const CallArg>> positional_args;
    std::unordered_map<std::string, const CallArg &> keyword_args;
};


class CallResult {
public:
    CallResult();

    explicit CallResult(const Object &);

    [[nodiscard]] bool success() const;


    struct CallError {
        const std::string message;

        explicit CallError(std::string message_) :
                message(std::move(message_)) {}
    };

    void add_call_error(std::string message);

    [[nodiscard]] const std::list<CallError> &call_errors() const;

    struct ArgError {
        const CallArg &arg;
        const std::string message;

        ArgError(const CallArg &arg_, std::string message_) :
                arg(arg_), message(std::move(message_)) {}

    };

    void add_arg_error(const CallArg &, std::string message);

    [[nodiscard]] const std::list<ArgError> &arg_errors() const;

    void set_return_value(const Object &object);

    [[nodiscard]] const Object &return_value() const;

private:
    const Object *_return_value;
    std::list<CallError> call_error_list;
    std::list<ArgError> arg_error_list;
};


class CallHandler {
public:
    [[nodiscard]] virtual CallResult call(const CallArgList &arguments) const = 0;

    [[nodiscard]] virtual bool operator==(const CallHandler &rhs) const = 0;
};

class Object {
public:
    [[nodiscard]] virtual const Object &attr(const std::string &id) const = 0;

    [[nodiscard]] virtual bool is_callable() const = 0;

    [[nodiscard]] virtual const CallHandler &get_call_handler() const = 0;

    [[nodiscard]] virtual const std::string &get_string() const = 0;

    [[nodiscard]] virtual const std::vector<std::reference_wrapper<const Object>> &entries() const = 0;
};

bool operator==(std::reference_wrapper<const Object>, std::reference_wrapper<const Object>);

class NullObject : public Object {
public:
    static Object &get_instance();

    static bool is_null(const Object &);

    [[nodiscard]] const Object &attr(const std::string &id) const override;

    [[nodiscard]] bool is_callable() const override;

    [[nodiscard]] const CallHandler &get_call_handler() const override;

    [[nodiscard]] const std::string &get_string() const override;

    [[nodiscard]] const std::vector<std::reference_wrapper<const Object>> &entries() const override;
};

class StructObject : public Object {
public:
    explicit StructObject(std::unordered_map<std::string, const Object &> attributes);

    [[nodiscard]] const Object &attr(const std::string &id) const override;

    [[nodiscard]] bool is_callable() const override;

    [[nodiscard]] const CallHandler &get_call_handler() const override;

    [[nodiscard]] const std::string &get_string() const override;

    [[nodiscard]] const std::vector<std::reference_wrapper<const Object>> &entries() const override;

private:
    const std::unordered_map<std::string, const Object &> attributes;
};


class FunctionObject : public Object {
public:
    explicit FunctionObject(CallHandler &handler);

    [[nodiscard]] const Object &attr(const std::string &id) const override;

    [[nodiscard]] bool is_callable() const override;

    [[nodiscard]] const CallHandler &get_call_handler() const override;

    [[nodiscard]] const std::string &get_string() const override;

    [[nodiscard]] const std::vector<std::reference_wrapper<const Object>> &entries() const override;

private:
    CallHandler &handler;
};


class StringObject : public Object {
public:
    explicit StringObject(std::string value);

    [[nodiscard]] const Object &attr(const std::string &id) const override;

    [[nodiscard]] bool is_callable() const override;

    [[nodiscard]] const CallHandler &get_call_handler() const override;

    [[nodiscard]] const std::string &get_string() const override;

    [[nodiscard]] const std::vector<std::reference_wrapper<const Object>> &entries() const override;

private:
    const std::string value;
};


class ListObject : public Object {
public:
    explicit ListObject(std::list<std::reference_wrapper<const Object>> entries);

    [[nodiscard]] const Object &attr(const std::string &id) const override;

    [[nodiscard]] bool is_callable() const override;

    [[nodiscard]] const CallHandler &get_call_handler() const override;

    [[nodiscard]] const std::string &get_string() const override;

    [[nodiscard]] const std::vector<std::reference_wrapper<const Object>> &entries() const override;

private:
    const std::vector<std::reference_wrapper<const Object>> _entries;
};

class ObjectStore {
public:
    virtual Object &create_struct(std::unordered_map<std::string, const Object &> attributes) = 0;

    virtual Object &create_function(CallHandler &handler) = 0;

    virtual Object &create_string(std::string value) = 0;

    virtual Object &create_list(std::list<std::reference_wrapper<const Object>> entries) = 0;
};


class UnknownAttributeError : public std::runtime_error {
public:
    explicit UnknownAttributeError(const std::string &attr_) :
            std::runtime_error("Unknown attribute '" + attr_ + "'"),
            attr(attr_) {};
private:
    const std::string attr;
};

class ObjectNotCallableError : public std::runtime_error {
public:
    explicit ObjectNotCallableError(const Object &object_) :
            std::runtime_error("Object is not callable"),
            object(object_) {};
private:
    const Object &object;
};

class ObjectIsNotAString : public std::runtime_error {
public:
    explicit ObjectIsNotAString(const Object &object_) :
            std::runtime_error("Object is not a string"),
            object(object_) {};
private:
    const Object &object;
};

class ObjectIsNotAList : public std::runtime_error {
public:
    explicit ObjectIsNotAList(const Object &object_) :
            std::runtime_error("Object is not a list"),
            object(object_) {};
private:
    const Object &object;
};

class MissingPositionalArgument : public std::runtime_error {
public:
    explicit MissingPositionalArgument() :
            std::runtime_error("Missing positional argument") {}
};

class MissingKeywordArgument : public std::runtime_error {
public:
    explicit MissingKeywordArgument() :
            std::runtime_error("Missing keyword argument") {}
};


//class AttributeAlreadyDefinedError : public std::runtime_error {
//public:
//    explicit AttributeAlreadyDefinedError(const std::string &attr) :
//            std::runtime_error("Attribute '" + attr + "' is already defined"),
//            attr(attr) {};
//private:
//    const std::string attr;
//};
