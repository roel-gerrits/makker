//
// Created by roel on 9/27/22.
//

#include "BasicObjectStore.h"

Object &BasicObjectStore::create_struct(std::unordered_map<std::string, const Object &> attributes) {
    return struct_objects.emplace_back(attributes);
}

Object &BasicObjectStore::create_function(CallHandler &handler) {
    return function_objects.emplace_back(handler);
}

Object &BasicObjectStore::create_string(std::string value) {
    return string_objects.emplace_back(std::move(value));
}

Object &BasicObjectStore::create_list(std::list<std::reference_wrapper<const Object>> entries) {
    return list_objects.emplace_back(entries);
}
