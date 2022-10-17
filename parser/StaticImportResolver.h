//
// Created by roel on 10/11/22.
//


#pragma once

#include "ImportResolver.h"
#include <unordered_map>

class StaticImportResolver : public ImportResolver {
public:
    StaticImportResolver();

    void set(const std::string &import_spec, Source &source);

    Result resolve(const std::string &import_spec) override;

private:
    std::unordered_map<std::string, Source &> import_map;
};


