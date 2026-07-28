#ifndef PARSER
#define PARSER

#include "ast.hpp"
#include "lexer.hpp"
#include <memory>
#include <vector>

class Parser {

    private:
        std::vector<Token>& tokens;
        size_t idx_currentToken = 0;

    public:
        std::unique_ptr<ASTNode> parse();

        Parser(std::vector<Token>& tokens) 
        : tokens(tokens), idx_currentToken(0) {}

    private:
        // parseAssignment(); // =
        // parseTerm(); // + -
        // parseFactor(); // * /
        // parsePower(); // ^
        // parseUnary(); // + -
        // parsePrimary(); 

        std::unique_ptr<ASTNode> parseStatement();
        std::unique_ptr<ASTNode> parseAssignment();
        std::unique_ptr<ASTNode> parseTerm();
        std::unique_ptr<ASTNode> parseFactor();
        std::unique_ptr<ASTNode> parsePower();
        std::unique_ptr<ASTNode> parseUnary();
        std::unique_ptr<ASTNode> parsePrimary();
        std::unique_ptr<ASTNode> parseFuncCall();

        // Helpers
        bool match(Token::TokenType type);
        ASTNode::TypeOp getASTop(Token& token);
        ASTNode::TypeNode getASTNode(Token& token);

};

#endif