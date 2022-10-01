//
// Created by roel on 7/25/22.
//

#include "Environment.h"

void Environment::define(const std::string &id, const Object &obj) {
    auto[it, is_inserted] = objects.emplace(id, obj);
    if (!is_inserted) {
        throw AlreadyDefinedError(id);
    }
    objects.emplace(id, obj);
}

const Object &Environment::get(const std::string &id) {
    auto it = objects.find(id);
    if (it == objects.end()) {
        throw UndefinedVariableError(id);
    }
    return it->second;
}

