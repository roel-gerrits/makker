//
// Created by roel on 7/17/22.
//

#include <iostream>
#include <string>
#include <ncurses.h>

#include "lexer/StringScanner.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"


int main() {

    ObjectStore store;
    Environment env;

    MultiImportResolver import_resolver;
    import_resolver.add(Filter("*.mkr"), MkrImportResolver());
    import_resolver.add(Filter("*.py"), PythonImportResolver(store));

    ResolveResult resolve_result = import_resolver.resolve("targets.mkr");
    resolve_result.success(); // --> bool
    resolve_result.type(); // --> PROGRAM / EXTERNAL_OBJECT
    resolve_result.get_source(); // --> Source& (only for type() == PROGRAM, else exception)
    ExternalObject& ext_obj = resolve_result.get_object(); // --> ExternalObject& (only for type() == EXTERNAL_OBJECT, else exception)

    ext_obj.initialize(); // ????

    Parser parser(import_resolver);

    while(true) {
        ParseResult parse_result = parser.parse(FileSource("targets.mkr"));

        ParseResult parse_result = parser.parse(StringSource("a=b"));

        InterpetResult interpret_result = interpret(store, env, parse_result.ast());
    }

}

