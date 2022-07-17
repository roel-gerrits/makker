//
// Created by roel on 5/30/22.
//

#include <memory>
#include "Parser.h"
#include "util/RewindableTokenStream.h"

static Token read(RewindableTokenStream &tokens, TokenType type) {
    const Token &t = tokens.next();
    if (t.type != type) {
        throw UnexpectedTokenError(t);
    }
    return t;
}

static Node parse_object(RewindableTokenStream &tokens);

static Node parse_expr(RewindableTokenStream &tokens);

/*
 * call_arg:
 *    : ID ASSIGN expr
 *    | expr
 */
static Node parse_call_arg(RewindableTokenStream &tokens) {
    auto snapshot = tokens.snapshot();
    try {
        read(tokens, TokenType::IDENTIFIER);
        read(tokens, TokenType::ASSIGN);
        return parse_expr(tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
    }

    return parse_expr(tokens);
}

/*
 * call_statement
 *      : object PAR_OPEN ( call_arg )* PAR_CLOSE
 */
static Node parse_call_statement(RewindableTokenStream &tokens) {
    Node node(NodeType::CALL_STATEMENT);
    node.add_child(parse_object(tokens));
    read(tokens, TokenType::PAR_OPEN);
    while (true) {
        try {
            node.add_child(parse_call_arg(tokens));
        } catch (const UnexpectedTokenError &e) {
            // ignore
            break;
        }
    }
    read(tokens, TokenType::PAR_CLOSE);

    return node;
}

/*
 * variable
 *      : ID
 */
static Node parse_variable(RewindableTokenStream &tokens) {
    Token token = read(tokens, TokenType::IDENTIFIER);
    return {NodeType::VARIABALE, token.value};
}

/*
 * object
 *      : ID [ DOT object ]
 */
static Node parse_object(RewindableTokenStream &tokens) {
    std::unique_ptr<Node> prev_node;

    while (true) {
        Token id = read(tokens, TokenType::IDENTIFIER);
        auto node = std::make_unique<Node>(NodeType::OBJECT, id.value);

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
 * list
 *      : BRACK_OPEN (expr)* BRACK_CLOSE
 */
static Node parse_list(RewindableTokenStream &tokens) {
    Node node(NodeType::LIST);
    read(tokens, TokenType::BRACK_OPEN);
    while (true) {
        try {
            node.add_child(parse_expr(tokens));
        } catch (const UnexpectedTokenError &e) {
            // ignore
            break;
        }
    }
    read(tokens, TokenType::BRACK_CLOSE);
    return node;
}

/*
 * list_for
 *      : BRACK_OPEN expr FOR variable IN expr BRACK_CLOSE
 */
static Node parse_list_for(RewindableTokenStream &tokens) {
    Node node(NodeType::LIST_FOR);
    read(tokens, TokenType::BRACK_OPEN);
    node.add_child(parse_expr(tokens));
    read(tokens, TokenType::FOR);
    node.add_child(parse_variable(tokens));
    read(tokens, TokenType::IN);
    node.add_child(parse_expr(tokens));
    read(tokens, TokenType::BRACK_CLOSE);
    return node;
}

/*
 * expr:
 *      : call_statement
 *      | object
 *      | PAR_OPEN expr PAR_CLOSE
 *      | list ## TODO
 *      | list_for ## TODO
 *      | STRING
 */
static Node parse_expr(RewindableTokenStream &tokens) {
    auto snapshot = tokens.snapshot();
    const Token *failure_token = nullptr;

    try {
        return parse_call_statement(tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        return parse_object(tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        read(tokens, TokenType::PAR_OPEN);
        Node node = parse_object(tokens);
        read(tokens, TokenType::PAR_CLOSE);
        return node;
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        return parse_list(tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        return parse_list_for(tokens);
    } catch (const UnexpectedTokenError &e) {
        tokens.rewind(snapshot);
        failure_token = &e.get_token();
    }

    try {
        Token token = read(tokens, TokenType::STRING);
        return {NodeType::STRING, token.value};
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
 * assignment_statement
 *    : variable ASSIGN expr
 */
static Node parse_assignment_statement(RewindableTokenStream &tokens) {
    Node node(NodeType::ASSIGNMENT_STATEMENT);
    node.add_child(parse_variable(tokens));
    read(tokens, TokenType::ASSIGN);
    node.add_child(parse_expr(tokens));
    return node;
}

/*
 * statement
 *      : assignment_statement
 *      | call_statement
 */
static Node parse_statement(RewindableTokenStream &tokens) {
    auto snapshot = tokens.snapshot();
    const Token *failure_token = nullptr;

    try {
        return parse_assignment_statement(tokens);
    } catch (const UnexpectedTokenError &e) {
        failure_token = &e.get_token();
        tokens.rewind(snapshot);
    }

    try {
        return parse_call_statement(tokens);
    } catch (const UnexpectedTokenError &e) {
        failure_token = &e.get_token();
        tokens.rewind(snapshot);
    }

    throw UnexpectedTokenError(*failure_token);
}

/*
 * program
 *      : ( statement )* EOS
 */
static Node parse_program(RewindableTokenStream &tokens) {
    Node node(NodeType::PROGRAM);

    while (true) {
        auto snapshot = tokens.snapshot();
        try {
            node.add_child(parse_statement(tokens));
        } catch (const UnexpectedTokenError &e) {
            tokens.rewind(snapshot);
            break;
        }
    }

    read(tokens, TokenType::EOS);
    return node;
}

Node parse(TokenStream &tokens) {
    RewindableTokenStream rewindable_tokens(tokens);
    return parse_program(rewindable_tokens);
}

