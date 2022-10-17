//
// Created by roel on 10/1/22.
//


#pragma once

#include "Scope.h"

class RootScope : public Scope {
public:
    [[nodiscard]] const Object &get(const std::string &variable) const override;

    void put(const std::string &variable, const Object &object) override;

    const std::unordered_map<std::string, const Object &>& get_map();
private:
    std::unordered_map<std::string, const Object &> objects;
};


