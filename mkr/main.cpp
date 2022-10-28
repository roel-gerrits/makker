//
// Created by roel on 7/17/22.
//


//#include "lexer/StringScanner.h"
//#include "lexer/Lexer.h"
//#include "parser/Parser.h"
////
//ObjectStore store;
//Environment env;
//
//MultiImportResolver import_resolver;
//import_resolver.add(Filter("*.mkr"), MkrImportResolver());
//import_resolver.add(Filter("*.py"), PythonImportResolver(store));
//
//ResolveResult resolve_result = import_resolver.resolve("targets.mkr");
//resolve_result.success(); // --> bool
//resolve_result.type(); // --> PROGRAM / EXTERNAL_OBJECT
//resolve_result.get_source(); // --> Source& (only for type() == PROGRAM, else exception)
//ExternalObject& ext_obj = resolve_result.get_object(); // --> ExternalObject& (only for type() == EXTERNAL_OBJECT, else exception)
//
//ext_obj.initialize(); // ????
//
//Parser parser(import_resolver);
//
//while(true) {
//ParseResult parse_result = parser.parse(FileSource("targets.mkr"));
//
//ParseResult parse_result = parser.parse(StringSource("a=b"));
//
//InterpetResult interpret_result = interpret(store, env, parse_result.ast());
//}

#include "Shell.h"
#include "Repl.h"
#include "parser/StringSource.h"
#include "parser/StaticImportResolver.h"


std::string prefix_lines(const std::string &src, const std::string &prefix) {
    std::string result = prefix;
    for (char ch: src) {
        result += ch;
        if (ch == '\n') {
            result += prefix;
        }
    }
    return result;
}

class SimpleShellHandler : public ShellHandler {
public:
    SimpleShellHandler(Shell &shell_, Repl &repl_) :
            shell(shell_),
            repl(repl_) {}

    void handle_up() override {

    }

    void handle_down() override {

    }

    void handle_tab() override {

    }

    void handle_line() override {
        StringSource source(shell.get_line());
        const auto result = repl.eval(source);

        if (!result.success()) {
            for (const Repl::EvalResult::Error &error: result.errors()) {
                shell.println("Error: " + error.msg);
                shell.println(prefix_lines(error.source_location.annotate("here"), "    "));
            }
        }
        shell.set_line("");
    }

    void handle_exit() override {
        shell.set_line("exit");
        shell.exit();
    }

private:
    Shell &shell;
    Repl &repl;
};

int main() {
    StaticImportResolver import_resolver;
    BasicObjectStore object_store;
    RootScope scope;



    Repl repl(import_resolver, object_store, scope);
    Shell shell;
    SimpleShellHandler handler(shell, repl);

    shell.run(handler);
}
