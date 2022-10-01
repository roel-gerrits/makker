//
// Created by roel on 10/1/22.
//

#include "ScopeWrapper.h"

ScopeWrapper::ScopeWrapper(const Scope &base_) :
        base(base_),
        objects() {}

const Object &ScopeWrapper::get(const std::string &variable) const {
    auto it = objects.find(variable);
    if (it != objects.end()) {
        return it->second;
    }
    return base.get(variable);
}

void ScopeWrapper::put(const std::string &variable, const Object &object) {
    auto[it, is_inserted] = objects.emplace(variable, object);
    if (!is_inserted) {
        throw AlreadyDefinedError(variable);
    }
}
