//
// Created by roel on 10/11/22.
//

#include "StaticImportResolver.h"

StaticImportResolver::StaticImportResolver() :
        import_map() {}

void StaticImportResolver::set(const std::string &import_spec, Source &source) {
    import_map.emplace(import_spec, source);
}

ImportResolver::Result StaticImportResolver::resolve(const std::string &import_spec) {
    auto it = import_map.find(import_spec);
    if (it == import_map.end()) {
        return Result(Result::Type::ERROR);
    }
    return {Result::Type::MKR_PROGRAM, it->second};
}
