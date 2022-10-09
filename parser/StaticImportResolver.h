//
// Created by roel on 10/6/22.
//


#pragma once

#include "ImportResolver.h"

#include <unordered_map>

class StaticImportResolver : public ImportResolver {
public:
    TokenStream& resolve(const std::string &import_spec) override;

    void set(const std::string &import_spec, TokenStream& tokenstream);

private:
    std::unordered_map<std::string, TokenStream&> import_map;
};


