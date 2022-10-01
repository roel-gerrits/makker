//
// Created by roel on 9/28/22.
//

#include "Object.h"

#include <utility>


/*
 * NullObject:*
 */

Object &NullObject::get_instance() {
    static NullObject obj;
    return obj;
}

bool NullObject::is_null(const Object &obj) {
    return &obj == &get_instance();
}

const Object &NullObject::attr(const std::string &id) const {
    throw UnknownAttributeError(id);
}

bool NullObject::is_callable() const {
    return false;
}

const CallHandler &NullObject::get_call_handler() const {
    throw ObjectNotCallableError(*this);
}

const std::string &NullObject::get_string() const {
    throw ObjectIsNotAString(*this);
}

const std::vector<std::reference_wrapper<const Object>> &NullObject::entries() const {
    throw ObjectIsNotAList(*this);
}


/*
 * StructObject::*
 */

StructObject::StructObject(std::unordered_map<std::string, const Object &> attributes_) :
        attributes(std::move(attributes_)) {}


const Object &StructObject::attr(const std::string &id) const {
    auto it = attributes.find(id);
    if (it == attributes.end()) {
        throw UnknownAttributeError(id);
    }
    return it->second;
}

bool StructObject::is_callable() const {
    return false;
}

const CallHandler &StructObject::get_call_handler() const {
    throw ObjectNotCallableError(*this);
}

const std::string &StructObject::get_string() const {
    throw ObjectIsNotAString(*this);
}

const std::vector<std::reference_wrapper<const Object>> &StructObject::entries() const {
    throw ObjectIsNotAList(*this);
}


/*
 * FunctionObject::*
 */


FunctionObject::FunctionObject(CallHandler &handler_) :
        handler(handler_) {}

const Object &FunctionObject::attr(const std::string &id) const {
    throw UnknownAttributeError(id);
}

bool FunctionObject::is_callable() const {
    return true;
}

const CallHandler &FunctionObject::get_call_handler() const {
    return handler;
}

const std::string &FunctionObject::get_string() const {
    throw ObjectIsNotAString(*this);
}

const std::vector<std::reference_wrapper<const Object>> &FunctionObject::entries() const {
    throw ObjectIsNotAList(*this);
}


/*
 * CallArgList::*
 */

CallArgList::CallArgList() :
        positional_args(),
        keyword_args() {}

void CallArgList::add(const CallArg &arg) {
    positional_args.emplace_back(arg);
}

void CallArgList::add(std::string keyword, const CallArg &arg) {
    keyword_args.emplace(std::move(keyword), arg);
}

const CallArg &CallArgList::arg(unsigned int index) const {
    if (index >= positional_args.size()) throw MissingPositionalArgument();
    return std::next(positional_args.begin(), index)->get();
}

const CallArg &CallArgList::arg(const std::string &key) const {
    auto it = keyword_args.find(key);
    if (it == keyword_args.end()) throw MissingKeywordArgument();
    return it->second;
}


/*
 * CallResult::*
 */

CallResult::CallResult() :
        CallResult(NullObject::get_instance()) {}

CallResult::CallResult(const Object &object_) :
        _return_value(&object_),
        call_error_list(),
        arg_error_list() {}

bool CallResult::success() const {
    return call_error_list.empty() && arg_error_list.empty();
}

void CallResult::add_call_error(std::string message) {
    call_error_list.emplace_back(std::move(message));
}

const std::list<CallResult::CallError> &CallResult::call_errors() const {
    return call_error_list;
}

void CallResult::add_arg_error(const CallArg &arg, std::string message) {
    arg_error_list.emplace_back(arg, std::move(message));
}

const std::list<CallResult::ArgError> &CallResult::arg_errors() const {
    return arg_error_list;
}

void CallResult::set_return_value(const Object &object) {
    _return_value = &object;
}

const Object &CallResult::return_value() const {
    return *_return_value;
}


/*
 * StringObject::*
 */

StringObject::StringObject(std::string value_) :
        value(std::move(value_)) {}

const Object &StringObject::attr(const std::string &id) const {
    throw UnknownAttributeError(id);
}

bool StringObject::is_callable() const {
    return false;
}

const CallHandler &StringObject::get_call_handler() const {
    throw ObjectNotCallableError(*this);
}

const std::string &StringObject::get_string() const {
    return value;
}

const std::vector<std::reference_wrapper<const Object>> &StringObject::entries() const {
    throw ObjectIsNotAList(*this);
}


/*
 * ListObject::*
 */

ListObject::ListObject(std::list<std::reference_wrapper<const Object>> entries) :
        _entries(entries.begin(), entries.end()) {}

const Object &ListObject::attr(const std::string &id) const {
    throw UnknownAttributeError(id);
}

bool ListObject::is_callable() const {
    return false;
}

const CallHandler &ListObject::get_call_handler() const {
    throw ObjectNotCallableError(*this);
}

const std::string &ListObject::get_string() const {
    throw ObjectIsNotAString(*this);
}

const std::vector<std::reference_wrapper<const Object>> &ListObject::entries() const {
    return _entries;
}
