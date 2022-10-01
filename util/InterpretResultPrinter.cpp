//
// Created by roel on 9/30/22.
//

#include "InterpretResultPrinter.h"

void print_interpret_result(const InterpretResult &result) {
    if (result.success()) {
        printf("Interpreting success :) \n");
    } else {
        printf("Interpreting failed :( \n");
        for (const InterpretResult::Error &error: result.errors()) {
            printf("  %u:%u: %s \n",
                   error.node.get_token().pos.line,
                   error.node.get_token().pos.col,
                   error.message.c_str());
        }
    }
}
