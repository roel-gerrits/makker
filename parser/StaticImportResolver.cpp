//
// Created by roel on 10/6/22.
//

#include "StaticImportResolver.h"

TokenStream &StaticImportResolver::resolve(const std::string &import_spec) {
    auto it = import_map.find(import_spec);
    if (it == import_map.end()) {
        throw ImportError(import_spec);
    }
    return it->second;
}

void StaticImportResolver::set(const std::string &import_spec, TokenStream &tokenstream) {
    import_map.emplace(import_spec, tokenstream);
}
