#include <memory>
#include "lexer.hpp"
#include "ast.hpp"

std::unique_ptr<ASTNode> ASTNode::ast_newInt(int64_t value) {

    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();
    node->type = TypeNode::INT;
    node->as.integer = value;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newFloat(double value) {

    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();
    node->type = TypeNode::FLOAT;
    node->as.real = value;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newString(std::string_view text) {

    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();
    node->type = TypeNode::STRING;
    node->as.text = text;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newBool(bool value) {

    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();
    node->type = TypeNode::BOOL;
    node->as.boolean = value;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newVar(std::string_view text) {

    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();
    node->type = TypeNode::VARIABLE;
    node->as.text = text;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newBinary(TypeOp op,
    std::unique_ptr<ASTNode> left,
    std::unique_ptr<ASTNode> right) {
    
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::BINARY_EXPR;
    node->op = op;

    node->children.push_back(std::move(left));
    node->children.push_back(std::move(right));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newUnary(TypeOp op,
    std::unique_ptr<ASTNode> operand) {

    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::UNARY_EXPR;
    node->op = op;

    node->children.push_back(std::move(operand));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newAssignment(
    std::unique_ptr<ASTNode> variable,
    std::unique_ptr<ASTNode> expression)
{
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::ASSIGNMENT;

    node->children.push_back(std::move(variable));
    node->children.push_back(std::move(expression));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newFunctionCall(
    std::unique_ptr<ASTNode> function)
{
    auto node = std::make_unique<ASTNode>();

    node->type = FUNCTION_CALL;
    node->children.push_back(std::move(function));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newNULL() {
    auto node = std::make_unique<ASTNode>();
    node->type = null;
    node->as.ptr = nullptr;
    
    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newProgram() {

    auto node = std::make_unique<ASTNode>();
    node->type = PROGRAM;
    node->as.ptr = nullptr;

    return node;
}

const char *typeToString(ASTNode::TypeNode type)
{
    switch (type)
    {
    case ASTNode::INT:           return "INT";
    case ASTNode::FLOAT:         return "FLOAT";
    case ASTNode::STRING:        return "STRING";
    case ASTNode::BOOL:          return "BOOL";
    case ASTNode::VARIABLE:      return "VARIABLE";
    case ASTNode::ASSIGNMENT:    return "ASSIGNMENT";
    case ASTNode::UNARY_EXPR:    return "UNARY_EXPR";
    case ASTNode::BINARY_EXPR:   return "BINARY_EXPR";
    case ASTNode::FUNCTION_CALL: return "FUNCTION_CALL";
    default:            return "UNKNOWN";
    }
}
const char *opToString(ASTNode::TypeOp op)
{
    switch (op)
    {
    case ASTNode::ADD: return "+";
    case ASTNode::SUB: return "-";
    case ASTNode::MUL: return "*";
    case ASTNode::DIV: return "/";
    case ASTNode::POW: return "^";
    case ASTNode::MOD: return "%";
    default: return "?";
    }
}

void ASTNode::dump(int indent) const
{
    for(int i = 0; i < indent; i++)
        std::cout << "    ";

    switch(type)
    {
        case INT:
            std::cout << "INT(" << as.integer << ")";
            break;

        case FLOAT:
            std::cout << "FLOAT(" << as.real << ")";
            break;

        case STRING:
            std::cout << "STRING(" << as.text << ")";
            break;

        case BOOL:
            std::cout << "BOOL(" << as.boolean << ")";
            break;

        case VARIABLE:
            std::cout << "VARIABLE(" << as.text << ")";
            break;

        case BINARY_EXPR:
            std::cout << "BINARY_EXPR op=" << op;
            break;

        case UNARY_EXPR:
            std::cout << "UNARY_EXPR op=" << op;
            break;

        case ASSIGNMENT:
            std::cout << "ASSIGNMENT";
            break;
        
        case FUNCTION_CALL:
            std::cout << "FUNCTION CALL";
            break;

        case PROGRAM:
            std::cout << "PROGRAM";
            break;

        default:
            std::cout << "UNKNOWN";
            break;
    }

    std::cout << '\n';


    for(const auto& child : children)
    {
        child->dump(indent + 1);
    }
}