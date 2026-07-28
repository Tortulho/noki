#include "parser.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include <memory>
#include <charconv>

#define current tokens[idx_currentToken]

std::unique_ptr<ASTNode> Parser::parse() {
    auto program = ASTNode::ast_newProgram();

    while (current.getType() != Token::END_OF_FILE)
    {
        // Ignorar linhas vazias
        while (current.getType() == Token::NEWLINE_or_SEMICOLON) idx_currentToken++;

        if (current.getType() == Token::END_OF_FILE) break;

        auto stmt = parseStatement();

        if (stmt == nullptr) return nullptr;

        program->children.push_back(std::move(stmt));

        // Consumir o separador de instruções
        if (current.getType() == Token::NEWLINE_or_SEMICOLON) idx_currentToken++;
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {

    if (current.getType() == (Token::TokenType::null)) return ASTNode::ast_newNULL();
    if (current.getType() == (Token::TokenType::EXIT)) exit(EXIT_SUCCESS);

    return parseAssignment();

}

std::unique_ptr<ASTNode> Parser::parseAssignment() {

    std::unique_ptr<ASTNode> left = parseTerm();
    if (left == nullptr) return nullptr;

    if (current.getType() == (Token::TokenType::EQUAL)) {
        idx_currentToken++;
        std::unique_ptr<ASTNode> value = parseAssignment();
        if (value == nullptr) return nullptr;
        return ASTNode::ast_newAssignment(std::move(left),std::move(value));
    }

    return left;

}



std::unique_ptr<ASTNode> Parser::parseTerm()
{
    std::unique_ptr<ASTNode> left = parseFactor();
    if (left == nullptr) return nullptr;

    //temos um binary
    while (getASTop(current) == ASTNode::ADD || getASTop(current) == ASTNode::SUB) {

        ASTNode::TypeOp op = getASTop(current);
        idx_currentToken++;
        std::unique_ptr<ASTNode> right = parseFactor();
        if (right == nullptr) return nullptr;

        //todo: cuidado, em c era memleak
        left = ASTNode::ast_newBinary(op,std::move(left),std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor()
{
    std::unique_ptr<ASTNode> left = parsePower();
    if (left == nullptr) return nullptr;

    //temos um binary
    while (getASTop(current) == ASTNode::MUL || getASTop(current) == ASTNode::DIV || 
           getASTop(current) == ASTNode::MOD) {

        ASTNode::TypeOp op = getASTop(current);
        idx_currentToken++;
        std::unique_ptr<ASTNode> right = parsePower();
        if (right == nullptr) return nullptr;

        left = ASTNode::ast_newBinary(op,std::move(left),std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parsePower()
{
    std::unique_ptr<ASTNode> left = parseUnary();
    if (left == nullptr) return nullptr;

    //temos um binary
    while (getASTop(current) == ASTNode::POW) {

        //ASTNode::TypeOp op = getASTop(current);
        idx_currentToken++;
        std::unique_ptr<ASTNode> right = parseUnary();

        left = ASTNode::ast_newBinary(ASTNode::POW,std::move(left),std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnary()
{
    ASTNode::TypeOp op = getASTop(current);

    if (op == ASTNode::POSITIVE || op == ASTNode::NEGATIVE) {

        //ASTNode::TypeOp op = getASTop(current);
        idx_currentToken++;
        auto operand = parseUnary();
        return ASTNode::ast_newUnary(op,std::move(operand));
    }

    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary()
{
    ASTNode::TypeNode op = getASTNode(current);
    std::string_view str = current.getString();

    switch (op)
    {
    case ASTNode::TypeNode::INT:
        {idx_currentToken++;
        int64_t num;
        auto result = std::from_chars(str.data(),str.data()+str.size(),num,10);
        if (result.ec == std::errc()) {
            return ASTNode::ast_newInt(num);
        }}
        break;
    case ASTNode::TypeNode::FLOAT:
        {idx_currentToken++;
        double num;
        auto result = std::from_chars(str.data(),str.data()+str.size(),num);
        if (result.ec == std::errc()) {
            return ASTNode::ast_newFloat(num);
        }}
        break;
    case ASTNode::TypeNode::BOOL:
        idx_currentToken++;
        if (str == "true") return ASTNode::ast_newBool(true);
        else return ASTNode::ast_newBool(false);
        break;
    case ASTNode::TypeNode::STRING:
        idx_currentToken++;
        return ASTNode::ast_newString(str);
        break;
    // case ASTNode::TypeNode::VARIABLE:
    //     idx_currentToken++;
    //     return ASTNode::ast_newVar(str);
    //     break;
    default:
        break;
    }
    if (current.getType() == Token::null) {
        idx_currentToken++;
        return ASTNode::ast_newNULL();
    }
    if (current.getType() == Token::OPENPAR) {
        idx_currentToken++;
        std::unique_ptr<ASTNode> node = parseAssignment();
        if (current.getType() == Token::CLOSEPAR) {
            idx_currentToken++;
            return node;
        } else {
            idx_currentToken++;
            return nullptr;
        }
    }
    if (current.getType() == Token::IDENTIFIER) {
        idx_currentToken++;
        if (current.getType() == Token::OPENPAR) {
            //FUNC-CALL
            auto call = ASTNode::ast_newFunctionCall(ASTNode::ast_newVar(str));
            idx_currentToken++;
            if (current.getType() == Token::CLOSEPAR) {
                return call;
            } else {
                while (true) {
                    auto arg = parseAssignment();
                    if (arg == nullptr) return nullptr;

                    call->children.push_back(std::move(arg));
                    //idx_currentToken++;

                    if (current.getType() == Token::COMMA) {
                        idx_currentToken++;
                        continue;
                    }

                    if (current.getType() == Token::CLOSEPAR) {
                        idx_currentToken++;
                        return call;
                    }

                    return nullptr; // erro de sintaxe
                }
            }
        } else {
            return ASTNode::ast_newVar(str);
        }


    }
    idx_currentToken++;
    return nullptr;
}



bool Parser::match(Token::TokenType type) {
    if (tokens[idx_currentToken].getType() == type) return true;
    return false;
}

ASTNode::TypeOp Parser::getASTop(Token& token) {

    if (token.getType() == Token::TokenType::BINARY_OPERATOR) {

        std::string_view strcut = token.getString();
        if (strcut == "+") {
            return ASTNode::TypeOp::ADD;
        }
        if (strcut == "-") {
            return ASTNode::TypeOp::SUB;
        }
        if (strcut == "*") {
            return ASTNode::TypeOp::MUL;
        }
        if (strcut == "/") {
            return ASTNode::TypeOp::DIV;
        }
        if (strcut == "%") {
            return ASTNode::TypeOp::MOD;
        }
        if (strcut == "^") {
            return ASTNode::TypeOp::POW;
        }
        return ASTNode::TypeOp::NONE;
        //TODO: IMPLEMENTAR COMPS AQUI

    } else if (token.getType() == Token::TokenType::UNARY_OPERATOR) {
        std::string_view strcut = token.getString();
        if (strcut == "+") {
            return ASTNode::TypeOp::POSITIVE;
        }
        if (strcut == "-") {
            return ASTNode::TypeOp::NEGATIVE;
        }
        std::cout << "ERROR: Invalid unary operator.\n";

        return ASTNode::TypeOp::NONE;
    }
    return ASTNode::TypeOp::NONE;

}

//CAN BE OPTIMIZED
ASTNode::TypeNode Parser::getASTNode(Token& token) {

    auto type = token.getType();

    switch (type) {
    case Token::INT:
        return ASTNode::INT;
    case Token::FLOAT:
        return ASTNode::FLOAT;
    case Token::STRING:
        return ASTNode::STRING;
    case Token::BOOL:
        return ASTNode::BOOL;
    case Token::IDENTIFIER:
        return ASTNode::VARIABLE;
    case Token::BINARY_OPERATOR:
        return ASTNode::BINARY_EXPR;
    case Token::null:
        return ASTNode::null;
    default:
        return ASTNode::TypeNode::INVALID;
    }

}
