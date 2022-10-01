//
// Created by roel on 9/27/22.
//


#pragma once

#include <list>

#include "Object.h"

class BasicObjectStore : public ObjectStore {
public:
    Object &create_struct(std::unordered_map<std::string, const Object &> attributes) override;

    Object &create_function(CallHandler& handler) override;

    Object &create_string(std::string value) override;

private:
    std::list<StructObject> struct_objects;
    std::list<FunctionObject> function_objects;
    std::list<StringObject> string_objects;
};


