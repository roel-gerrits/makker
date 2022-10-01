//
// Created by roel on 6/18/22.
//

#include "AstSerializer.h"

std::string ast_to_string(const Node &node) {
    std::string result;
    result.append(to_str(node.get_type()));
    if (!node.get_token().value.empty()) {
        result.append(":" + node.get_token().value);
    }
    if (!node.get_children().empty()) {
        result.append("( ");
        for (const Node &child: node.get_children()) {
            result.append(ast_to_string(child));
            result.append(" ");
        }
        result.append(")");
    }
    return result;
}
