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

std::unique_ptr<ASTNode> Parser::parseFuncDecl()
{
    // consumir "function"
    idx_currentToken++;

    // nome da função
    if (current.getType() != Token::IDENTIFIER) return nullptr;

    auto name = ASTNode::ast_newVar(
        current.getString()
    );

    idx_currentToken++;

    if (current.getType() != Token::OPENPAR) return nullptr;

    idx_currentToken++;

    std::vector<FunctionParameter> parameters;

    // função sem argumentos
    if (current.getType() == Token::CLOSEPAR)
    {
        idx_currentToken++;
    }
    else
    {
        while (true)
        {
            FunctionParameterMode parameterMode =
                FunctionParameterMode::NORMAL;

            ASTNode::TypeNode parameterType =
                ASTNode::TypeNode::ANYVALUE;

            std::string parameterName;

            /*
             * Qualificador:
             *
             * mut
             * const
             */
            if (current.getType() == Token::MUTABLE)
            {
                parameterMode =
                    FunctionParameterMode::MUT;

                idx_currentToken++;
            }
            else if (current.getType() == Token::CONST)
            {
                parameterMode =
                    FunctionParameterMode::CONST;

                idx_currentToken++;
            }

            /*
             * const: a
             * mut: a
             */
            if (current.getType() == Token::COLON)
            {
                idx_currentToken++;

                if (current.getType() != Token::IDENTIFIER)
                    return nullptr;

                parameterName =
                    std::string(current.getString());

                idx_currentToken++;
            }
            else
            {
                /*
                 * a
                 * int: a
                 * mut int: a
                 * const int: a
                 */
                if (current.getType() != Token::IDENTIFIER)
                    return nullptr;

                std::string_view first =
                    current.getString();

                idx_currentToken++;

                // int: a
                if (current.getType() == Token::COLON)
                {
                    if (first == "int")
                        parameterType =
                            ASTNode::TypeNode::INT;
                    else if (first == "float")
                        parameterType =
                            ASTNode::TypeNode::FLOAT;
                    else if (first == "string")
                        parameterType =
                            ASTNode::TypeNode::STRING;
                    else if (first == "bool")
                        parameterType =
                            ASTNode::TypeNode::BOOL;
                    else if (first == "vector")
                        parameterType =
                            ASTNode::TypeNode::VECTOR;
                    else
                        return nullptr;

                    idx_currentToken++;

                    if (current.getType() != Token::IDENTIFIER)
                        return nullptr;

                    parameterName =
                        std::string(current.getString());

                    idx_currentToken++;
                }
                // a
                else
                {
                    /*
                     * Se tivermos um qualificador, o primeiro
                     * identifier tem obrigatoriamente de ser um
                     * tipo.
                     */
                    if (
                        parameterMode !=
                        FunctionParameterMode::NORMAL
                    )
                    {
                        return nullptr;
                    }

                    parameterName =
                        std::string(first);
                }
            }

            parameters.emplace_back(
                std::move(parameterName),
                parameterType,
                parameterMode
            );

            if (current.getType() == Token::COMMA)
            {
                idx_currentToken++;
                continue;
            }

            if (current.getType() == Token::CLOSEPAR)
            {
                idx_currentToken++;
                break;
            }

            return nullptr;
        }
    }

    // corpo
    auto body = parseBlock();

    if (body == nullptr) return nullptr;

    return ASTNode::ast_newFunctionDeclaration(
        std::move(name),
        std::move(parameters),
        std::move(body)
    );
}

std::unique_ptr<ASTNode> Parser::parseStatement() {

    if (current.getType() == (Token::TokenType::null)) return ASTNode::ast_newNULL();
    //if (current.getType() == (Token::TokenType::EXIT)) exit(EXIT_SUCCESS);
    if (current.getType() == Token::OPEN_SCOPE) return parseBlock();
    if (current.getType() == Token::IF) return parseIf();
    if (current.getType() == Token::WHILE) return parseWhile();
    if (current.getType() == Token::IMPORT) return parseImport();
    if (current.getType() == Token::FUNCTION) return parseFuncDecl();

    if (current.getType() == Token::BREAK) {
        idx_currentToken++;
        auto node = std::make_unique<ASTNode>();
        node->type = ASTNode::TypeNode::BREAK;
        return node;
    }

    if (current.getType() == Token::CONTINUE) {
        idx_currentToken++;
        auto node = std::make_unique<ASTNode>();
        node->type = ASTNode::TypeNode::CONTINUE;
        return node;
    }

    if (current.getType() == Token::RETURN)
    {
        idx_currentToken++;

        auto node = std::make_unique<ASTNode>();
        node->type = ASTNode::TypeNode::RETURN;

        // return sem expressão
        if (current.getType() == Token::NEWLINE_or_SEMICOLON ||
            current.getType() == Token::CLOSE_SCOPE ||
            current.getType() == Token::END_OF_FILE)
        {
            return node;
        }

        // return com expressão
        auto expression = parseAssignment();

        if (expression == nullptr)
            return nullptr;

        node->children.push_back(
            std::move(expression)
        );

        return node;
    }

    if (current.getType() == Token::EXIT) {
        idx_currentToken++;

        auto node = std::make_unique<ASTNode>();
        node->type = ASTNode::TypeNode::EXIT;

        return node;
    }


    return parseAssignment();

}

std::unique_ptr<ASTNode> Parser::parseMemberAccess()
{
    std::unique_ptr<ASTNode> left = parsePrimary();

    if (left == nullptr) return nullptr;

    while (true)
    {
        // Member access: object.member
        if (current.getType() == Token::DOT)
        {
            idx_currentToken++;

            if (current.getType() != Token::IDENTIFIER)
                return nullptr;

            auto member = ASTNode::ast_newVar(current.getString());

            idx_currentToken++;

            left = ASTNode::ast_newMemberAccess(
                std::move(left),
                std::move(member)
            );

            continue;
        }

        // Function call: expression(...)
        if (current.getType() == Token::OPENPAR)
        {
            idx_currentToken++;

            auto call = ASTNode::ast_newFunctionCall(
                std::move(left)
            );

            if (current.getType() == Token::CLOSEPAR)
            {
                idx_currentToken++;
                left = std::move(call);
                continue;
            }

            while (true)
            {
                auto arg = parseAssignment();

                if (arg == nullptr)
                    return nullptr;

                call->children.push_back(std::move(arg));

                if (current.getType() == Token::COMMA)
                {
                    idx_currentToken++;
                    continue;
                }

                if (current.getType() == Token::CLOSEPAR)
                {
                    idx_currentToken++;
                    left = std::move(call);
                    break;
                }

                return nullptr;
            }

            continue;
        }

        // Index: expression[index]
        if (current.getType() == Token::OPEN_BRACKET)
        {
            idx_currentToken++;

            auto index = parseAssignment();

            if (index == nullptr)
                return nullptr;

            if (current.getType() != Token::CLOSE_BRACKET)
                return nullptr;

            idx_currentToken++;

            left = ASTNode::ast_newIndex(
                std::move(left),
                std::move(index)
            );

            continue;
        }

        break;
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseIf()
{
    // consumir "if" / "elif"
    idx_currentToken++;

    auto condition = parseAssignment();
    if (condition == nullptr) return nullptr;

    auto trueBlock = parseBlock();
    if (trueBlock == nullptr) return nullptr;

    //existe elif?
    if (current.getType() == Token::ELIF)
    {
        auto falseBlock = parseIf();
        if (falseBlock == nullptr) return nullptr;

        return ASTNode::ast_newIf(
            std::move(condition),
            std::move(trueBlock),
            std::move(falseBlock)
        );
    }

    // existe else?
    if (current.getType() == Token::ELSE)
    {
        idx_currentToken++; // consumir else

        auto falseBlock = parseBlock();
        if (falseBlock == nullptr) return nullptr;

        return ASTNode::ast_newIf(
            std::move(condition),
            std::move(trueBlock),
            std::move(falseBlock));
    }

    // if sem else
    return ASTNode::ast_newIf(
        std::move(condition),
        std::move(trueBlock),
        nullptr);
}

std::unique_ptr<ASTNode> Parser::parseWhile()
{
    // consumir while
    idx_currentToken++;

    auto condition = parseAssignment();

    if (condition == nullptr) return nullptr;

    auto block = parseBlock();

    if (block == nullptr) return nullptr;

    return ASTNode::ast_newWhile(
        std::move(condition),
        std::move(block));
}

std::unique_ptr<ASTNode> Parser::parseBlock()
{
    if (current.getType() != Token::OPEN_SCOPE) return nullptr;

    idx_currentToken++; // consumir {

    auto block = ASTNode::ast_newBlock();

    while (current.getType() != Token::CLOSE_SCOPE
        && current.getType() != Token::END_OF_FILE)
    {
        auto statement = parseStatement();

        if (statement == nullptr) return nullptr;

        block->children.push_back(std::move(statement));

        // se NEXTLINE:
        if (current.getType() == Token::NEWLINE_or_SEMICOLON) idx_currentToken++;
    }

    //se tivermos fim de scope sem o indicativo devemos return null, ou seja, a operaçao nao tem valor
    if (current.getType() != Token::CLOSE_SCOPE) return nullptr;

    idx_currentToken++; // consumir }

    return block;
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    std::unique_ptr<ASTNode> left = parseComp();

    if (left == nullptr)
        return nullptr;

    // atribuição normal
    if (current.getType() == Token::TokenType::EQUAL)
    {
        idx_currentToken++;

        std::unique_ptr<ASTNode> value = parseAssignment();

        if (value == nullptr)
            return nullptr;

        return ASTNode::ast_newAssignment(
            std::move(left),
            std::move(value)
        );
    }

    if (current.getType() == Token::TokenType::MOVE)
    {
        idx_currentToken++;

        std::unique_ptr<ASTNode> value = parseAssignment();

        if (value == nullptr) return nullptr;

        return ASTNode::ast_newMove(
            std::move(left),
            std::move(value)
        );
    }

    //patch de fita adesiva
    // atribuição composta
    if (current.getType() == Token::TokenType::BINARY_OPERATOR)
    {
        std::string_view op = current.getString();

        ASTNode::TypeOp binaryOp = ASTNode::TypeOp::NONE;

        if (op == "+=")
            binaryOp = ASTNode::TypeOp::ADD;
        else if (op == "-=")
            binaryOp = ASTNode::TypeOp::SUB;
        else if (op == "*=")
            binaryOp = ASTNode::TypeOp::MUL;
        else if (op == "/=")
            binaryOp = ASTNode::TypeOp::DIV;
        else if (op == "%=")
            binaryOp = ASTNode::TypeOp::MOD;
        else if (op == "^=")
            binaryOp = ASTNode::TypeOp::POW;

        if (binaryOp != ASTNode::TypeOp::NONE)
        {
            auto leftCopy = ASTNode::ast_clone(*left);

            idx_currentToken++;

            std::unique_ptr<ASTNode> value =
                parseAssignment();

            if (value == nullptr)
                return nullptr;

            auto expression =
                ASTNode::ast_newBinary(
                    binaryOp,
                    std::move(leftCopy),
                    std::move(value)
                );

            return ASTNode::ast_newAssignment(
                std::move(left),
                std::move(expression)
            );
        }
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseComp() {
    std::unique_ptr<ASTNode> left = parseTerm();
    if (left == nullptr) return nullptr;

    while (true)
    {
        ASTNode::TypeOp op = getASTop(current);

        if (op != ASTNode::EQUAL &&
            op != ASTNode::NOT_EQUAL &&
            op != ASTNode::LESS &&
            op != ASTNode::LESS_EQUAL &&
            op != ASTNode::GREATER &&
            op != ASTNode::GREATER_EQUAL &&
            op != ASTNode::AND &&
            op != ASTNode::OR)
        {
            break;
        }

        idx_currentToken++;

        std::unique_ptr<ASTNode> right = parseTerm();
        if (right == nullptr) return nullptr;

        left = ASTNode::ast_newBinary(op,std::move(left),std::move(right));
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
    if (current.getType() == Token::BINARY_OPERATOR ||
        current.getType() == Token::UNARY_OPERATOR)
    {
        std::string_view str = current.getString();

        if (str == "+" || str == "-")
        {
            ASTNode::TypeOp op =
                (str == "+")
                ? ASTNode::POSITIVE
                : ASTNode::NEGATIVE;

            idx_currentToken++;

            auto operand = parseUnary();

            if (operand == nullptr) return nullptr;

            return ASTNode::ast_newUnary(
                op,
                std::move(operand)
            );
        }

        if (str == "!")
        {
            idx_currentToken++;

            auto operand = parseUnary();

            if (operand == nullptr) return nullptr;

            return ASTNode::ast_newUnary(
                ASTNode::NEGATIVE,
                std::move(operand)
            );
        }
    }

    return parseMemberAccess();
}

std::unique_ptr<ASTNode> Parser::parseImport()
{
    idx_currentToken++;

    if (current.getType() != Token::IDENTIFIER)
        return nullptr;

    std::string_view name = current.getString();

    idx_currentToken++;

    return ASTNode::ast_newImport(name);
}

std::unique_ptr<ASTNode> Parser::parsePrimary()
{
    std::string_view str = current.getString();

    switch (current.getType())
    {
    case Token::INT:
    {
        idx_currentToken++;

        int64_t num;

        auto result = std::from_chars(
            str.data(),
            str.data() + str.size(),
            num,
            10
        );

        if (result.ec == std::errc())
            return ASTNode::ast_newInt(num);

        return nullptr;
    }

    case Token::FLOAT:
    {
        idx_currentToken++;

        double num;

        auto result = std::from_chars(
            str.data(),
            str.data() + str.size(),
            num
        );

        if (result.ec == std::errc())
            return ASTNode::ast_newFloat(num);

        return nullptr;
    }

    case Token::BOOL:
        idx_currentToken++;

        if (str == "true")
            return ASTNode::ast_newBool(true);

        return ASTNode::ast_newBool(false);

    case Token::STRING:
        idx_currentToken++;
        return ASTNode::ast_newString(str);

    default:
        break;
    }

    if (current.getType() == Token::null)
    {
        idx_currentToken++;
        return ASTNode::ast_newNULL();
    }

    if (current.getType() == Token::OPENPAR)
    {
        idx_currentToken++;

        std::unique_ptr<ASTNode> node =
            parseAssignment();

        if (current.getType() == Token::CLOSEPAR)
        {
            idx_currentToken++;
            return node;
        }

        return nullptr;
    }

    if (current.getType() == Token::IDENTIFIER)
    {
        idx_currentToken++;
        return ASTNode::ast_newVar(str);
    }

    if (current.getType() == Token::OPEN_BRACKET)
    {
        idx_currentToken++;
        return parseVector();
    }

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseVector()
{
    auto vector = ASTNode::ast_newVector();

    if (current.getType() == Token::CLOSE_BRACKET)
    {
        idx_currentToken++; // ]
        return vector;
    }

    while (true)
    {
        auto element = parseAssignment();

        if (element == nullptr) return nullptr;

        vector->children.push_back(std::move(element));

        if (current.getType() == Token::COMMA)
        {
            idx_currentToken++;
            continue;
        }

        if (current.getType() == Token::CLOSE_BRACKET)
        {
            idx_currentToken++; // ]
            return vector;
        }

        return nullptr;
    }
}

//deprecated
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
        if (strcut == "!")
            return ASTNode::TypeOp::NEGATIVE;
        if (strcut == "==")
            return ASTNode::TypeOp::EQUAL;
        if (strcut == "!=")
            return ASTNode::TypeOp::NOT_EQUAL;
        if (strcut == "<")
            return ASTNode::TypeOp::LESS;
        if (strcut == "<=")
            return ASTNode::TypeOp::LESS_EQUAL;
        if (strcut == ">")
            return ASTNode::TypeOp::GREATER;
        if (strcut == ">=")
            return ASTNode::TypeOp::GREATER_EQUAL;
        if (strcut == "&&")
            return ASTNode::TypeOp::AND;
        if (strcut == "||")
            return ASTNode::TypeOp::OR;
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
        if (strcut == "!") {
            return ASTNode::TypeOp::NEGATIVE;
        }
        std::cout << "ERROR: Invalid unary operator.\n";

        return ASTNode::TypeOp::NONE;
    }
    return ASTNode::TypeOp::NONE;

}
