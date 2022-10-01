//
// Created by roel on 10/1/22.
//

#include "RootScope.h"

const Object &RootScope::get(const std::string &variable) const {
    auto it = objects.find(variable);
    if (it == objects.end()) {
        throw UndefinedVariableError(variable);
    }
    return it->second;
}

void RootScope::put(const std::string &variable, const Object &object) {
    auto[it, is_inserted] = objects.emplace(variable, object);
    if (!is_inserted) {
        throw AlreadyDefinedError(variable);
    }
}
