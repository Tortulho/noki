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

std::unique_ptr<ASTNode> ASTNode::ast_newBlock() {
    auto node = std::make_unique<ASTNode>();
    node->type = BLOCK;
    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newIf(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> trueBlock,
    std::unique_ptr<ASTNode> falseBlock) {
    
        /*
        if cond {trueblock} else {falseblock}
        */

    auto node = std::make_unique<ASTNode>();

    node->type = IF;

    node->children.push_back(std::move(condition));
    node->children.push_back(std::move(trueBlock));

    if (falseBlock) node->children.push_back(std::move(falseBlock));

    return node;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newWhile(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> block)
{
    auto node = std::make_unique<ASTNode>();

    node->type = ASTNode::WHILE;

    node->children.push_back(std::move(condition));
    node->children.push_back(std::move(block));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newImport(std::string_view name)
{
    auto node = std::make_unique<ASTNode>();

    node->type = IMPORT;
    node->as.text = name;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newMemberAccess(
    std::unique_ptr<ASTNode> object,
    std::unique_ptr<ASTNode> member)
{
    auto node = std::make_unique<ASTNode>();

    node->type = ASTNode::MEMBER_ACCESS;

    node->children.push_back(std::move(object));
    node->children.push_back(std::move(member));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newVector()
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::VECTOR;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newIndex(
    std::unique_ptr<ASTNode> object,
    std::unique_ptr<ASTNode> index)
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::INDEX;

    node->children.push_back(std::move(object));
    node->children.push_back(std::move(index));

    return node;
}


/*
to parser:
//without else
return ASTNode::ast_newIf(
    std::move(condition),
    std::move(trueBlock),
    nullptr);

//with else
return ASTNode::ast_newIf(
    std::move(condition),
    std::move(trueBlock),
    std::move(falseBlock));

*/

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
    case ASTNode::VECTOR:        return "VECTOR";
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

        case BLOCK:
            std::cout << "BLOCK";
            break;

        case TypeNode::WHILE:
            std::cout << "WHILE";
            break;

        case TypeNode::IMPORT:
            std::cout << "IMPORT(" << as.text << ")";
            break;
            
        case MEMBER_ACCESS:
            std::cout << "MEMBER ACCESS";
            break;
        
        case VECTOR:
            std::cout << "VECTOR";
            break;

        case INDEX:
            std::cout << "INDEX";
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

std::unique_ptr<ASTNode> ASTNode::ast_clone(const ASTNode& node)
{
    auto copy = std::make_unique<ASTNode>();

    copy->type = node.type;
    copy->op = node.op;
    copy->as = node.as;

    for (const auto& child : node.children)
    {
        if (child != nullptr)
            copy->children.push_back(ast_clone(*child));
    }

    return copy;
}