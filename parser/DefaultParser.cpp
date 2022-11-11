//
// Created by roel on 10/11/22.
//

#include "DefaultParser.h"

#include "Lexer.h"
#include <memory>

DefaultParser::DefaultParser(ImportResolver &import_resolver_) :
        import_resolver(import_resolver_) {}


class ParseError : public std::exception {
};


DefaultParser::Result DefaultParser::parse(Source &source) {
    Result result;
    Lexer lexer(source);
    RewindableTokenStream rewindable_tokens(lexer);

    try {
        Node node = parse_program(result, rewindable_tokens);
        result.set_ast(node);
    } catch (const ParseError &) {
        // ignore
    } catch (const UnexpectedCharacter &e) {
        result.add_error(e.get_position(), e.what());
    } catch (const UnexpectedEnd &e) {
        result.add_error(e.get_position(), e.what());
    }

    return result;
}

/*
 * program
 *      : ( statement )* EOS
 */
Node DefaultParser::parse_program(Result &result, RewindableTokenStream &tokens) {
    Node node(NodeType::PROGRAM, tokens.peek().location);

    while (true) {
        auto snapshot = tokens.snapshot();
        try {
            node.add_child(parse_statement(result, tokens));
        } catch (const UnexpectedTokenError &e) {
            tokens.rewind(snapshot);
            break;
        }
    }

    try {
        read(tokens, TokenType::EOS);
        return node;
    } catch (const UnexpectedTokenError &e) {
        result.add_error(tokens.peek().location, "Unexpected EOS");
        throw ParseError();
    }
}

/*
 * statement
 *      : assignment_statement
 *      | call_statement
 */
Node DefaultParser::parse_statement(Result &result, RewindableTokenStream &tokens) {
    auto snapshot = tokens.snapshot();
    const Token *failure_token = nullptr;

    try {
        return parse_assignment_statement(result, tokens);
    } catch (const UnexpectedTokenError &e) {
        failure_token = &e.get_token();
        tokens.rewind(snapshot);
    }

    try {
        return parse_call_statement(result, tokens);
    } catch (const UnexpectedTokenError &e) {
        failure_token = &e.get_token();
        tokens.rewind(snapshot);
    }

    throw UnexpectedTokenError(*failure_token);
}

/*
 * assignment_statement
 *    : variable ASSIGN expr
 */
Node DefaultParser::parse_assignment_statement(Result &result, RewindableTokenStream &tokens) {
    Node node(NodeType::ASSIGNMENT_STATEMENT, tokens.peek().location);
    node.add_child(parse_variable(result, tokens));
    read(tokens, TokenType::ASSIGN);
    node.add_child(parse_expr(result, tokens));

    return node;
}

/*
 * expr:
 *      : import_statement
 *      | call_statement
 *      | object
 *      | PAR_OPEN expr PAR_CLOSE
 *      | list
 *      | list_for
 *      | STRING
 */
Node DefaultParser::parse_expr(Result &result, RewindableTokenStream &tokens) {
    auto snapshot = tokens.snapshot();
    const Token *failure_token = nullptr;

    try {
        return parse_import_statement(result, tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        return parse_call_statement(result, tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        return parse_object(result, tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        read(tokens, TokenType::PAR_OPEN);
        Node node = parse_object(result, tokens);
        read(tokens, TokenType::PAR_CLOSE);
        return node;
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        return parse_list(result, tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        return parse_list_for(result, tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        Token token = read(tokens, TokenType::STRING);
        return {NodeType::STRING, token.location, token.value};
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    if (!failure_token) {
        failure_token = &tokens.next();
    }

    throw UnexpectedTokenError(*failure_token);
}

/*
 * list_for
 *      : BRACK_OPEN expr FOR variable IN expr BRACK_CLOSE
 */
Node DefaultParser::parse_list_for(Result &result, RewindableTokenStream &tokens) {
    Node node(NodeType::LIST_FOR, tokens.peek().location);
    read(tokens, TokenType::BRACK_OPEN);
    node.add_child(parse_expr(result, tokens));
    read(tokens, TokenType::FOR);
    node.add_child(parse_variable(result, tokens));
    read(tokens, TokenType::IN);
    node.add_child(parse_expr(result, tokens));
    read(tokens, TokenType::BRACK_CLOSE);
    return node;
}

/*
 * list
 *      : BRACK_OPEN (expr)* BRACK_CLOSE
 */
Node DefaultParser::parse_list(Result &result, RewindableTokenStream &tokens) {
    Node node(NodeType::LIST, tokens.peek().location);
    read(tokens, TokenType::BRACK_OPEN);
    while (true) {
        try {
            node.add_child(parse_expr(result, tokens));
        } catch (const UnexpectedTokenError &e) {
            // ignore
            break;
        }
    }
    read(tokens, TokenType::BRACK_CLOSE);
    return node;
}

/*
 * object
 *      : ID [ DOT object ]
 */
Node DefaultParser::parse_object(Result &result, RewindableTokenStream &tokens) {
    std::unique_ptr<Node> prev_node;

    while (true) {
        Token id = read(tokens, TokenType::IDENTIFIER);
        auto node = std::make_unique<Node>(NodeType::OBJECT, id.location, id.value);

        if (prev_node) {
            node->add_child(std::move(*prev_node));
            prev_node.reset();
        }

        prev_node = std::move(node);

        auto snapshot = tokens.snapshot();
        try {
            read(tokens, TokenType::DOT);
        } catch (const UnexpectedTokenError &e) {
            tokens.rewind(snapshot);
            break;
        }
    }

    return *prev_node;
}

/*
 * variable
 *      : ID
 */
Node DefaultParser::parse_variable(Result &result, RewindableTokenStream &tokens) {
    Token token = read(tokens, TokenType::IDENTIFIER);
    return {NodeType::VARIABLE, token.location, token.value};
}


/*
 * import_statement
 *      : "import" PAR_OPEN STRING PAR_CLOSE
 */
Node DefaultParser::parse_import_statement(Result &result, RewindableTokenStream &tokens) {
    const Token identifier = read(tokens, TokenType::IDENTIFIER);
    if (identifier.value != "import") {
        throw UnexpectedTokenError(identifier);
    }

    read(tokens, TokenType::PAR_OPEN);

    const Token target = read(tokens, TokenType::STRING);

    read(tokens, TokenType::PAR_CLOSE);

    ImportResolver::Result import_result = import_resolver.resolve(target.value);
    if (!import_result.success()) {
        result.add_error(identifier.location, "Import failed");
        throw ParseError();
    }

    if (import_result.is_mkr_program()) {
        Lexer lexer(import_result.get_source());
        RewindableTokenStream token_stream(lexer);
        Node node = parse_program(result, token_stream);
        return node;
    }

    result.add_error(identifier.location, "Unknown import type");
    throw ParseError();
}


/*
 * call_statement
 *      : object PAR_OPEN ( call_arg )* PAR_CLOSE
 */
Node DefaultParser::parse_call_statement(Result &result, RewindableTokenStream &tokens) {
    Node node(NodeType::CALL_STATEMENT, tokens.peek().location);
    node.add_child(parse_object(result, tokens));
    read(tokens, TokenType::PAR_OPEN);
    while (true) {
        try {
            node.add_child(parse_call_arg(result, tokens));
        } catch (const UnexpectedTokenError &e) {
            // ignore
            break;
        }
    }
    read(tokens, TokenType::PAR_CLOSE);

    return node;
}

/*
 * call_arg:
 *    : ID ASSIGN expr
 *    | expr
 */
Node DefaultParser::parse_call_arg(Result &result, RewindableTokenStream &tokens) {
    auto snapshot = tokens.snapshot();
    try {
        Token id = read(tokens, TokenType::IDENTIFIER);
        Node node(NodeType::KWARG, id.location, id.value);
        read(tokens, TokenType::ASSIGN);
        node.add_child(parse_expr(result, tokens));
        return node;
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
    }

    return parse_expr(result, tokens);
}

const Token &DefaultParser::read(RewindableTokenStream &tokens, TokenType type) {
    const Token &t = tokens.next();
    if (t.type != type) {
        throw UnexpectedTokenError(t);
    }
    return t;
}
