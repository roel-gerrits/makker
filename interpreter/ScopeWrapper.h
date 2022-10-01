//
// Created by roel on 10/1/22.
//


#pragma once

#include "Scope.h"

class ScopeWrapper : public Scope {
public:
    explicit ScopeWrapper(const Scope &base);

    [[nodiscard]] const Object &get(const std::string &variable) const override;

    void put(const std::string &variable, const Object &object) override;

private:
    const Scope &base;
    std::unordered_map<std::string, const Object &> objects;
};


