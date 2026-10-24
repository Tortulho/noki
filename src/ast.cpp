#include <memory>
#include "lexer.hpp"
#include "ast.hpp"

std::unique_ptr<ASTNode> ASTNode::ast_newInt(int64_t value)
{
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::INT;
    node->as = value;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newFloat(double value)
{
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::FLOAT;
    node->as = value;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newString(std::string_view text)
{
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::STRING;
    node->as = text;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newBool(bool value)
{
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::BOOL;
    node->as = value;

    return node;
}

//CLASSES

std::unique_ptr<ASTNode> ASTNode::ast_newClassDeclaration(
    std::unique_ptr<ASTNode> name)
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::CLASS_DECLARATION;
    node->as = static_cast<void*>(nullptr);

    node->children.push_back(std::move(name));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newClassFieldDeclaration(
    TypeNode fieldType,
    std::unique_ptr<ASTNode> name,
    std::unique_ptr<ASTNode> defaultValue,
    AccessModifier accessModifier
)
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::CLASS_FIELD_DECLARATION;
    node->as = fieldType;
    node->accessModifier = accessModifier;

    node->children.push_back(std::move(name));
    node->children.push_back(std::move(defaultValue));

    return node;
}


//

std::unique_ptr<ASTNode> ASTNode::ast_newVar(std::string_view text)
{
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::VARIABLE;
    node->as = text;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newBinary(
    TypeOp op,
    std::unique_ptr<ASTNode> left,
    std::unique_ptr<ASTNode> right)
{
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();

    node->type = TypeNode::BINARY_EXPR;
    node->op = op;

    node->children.push_back(std::move(left));
    node->children.push_back(std::move(right));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newUnary(
    TypeOp op,
    std::unique_ptr<ASTNode> operand)
{
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

std::unique_ptr<ASTNode> ASTNode::ast_newMove(
    std::unique_ptr<ASTNode> variable,
    std::unique_ptr<ASTNode> expression
)
{
    std::unique_ptr<ASTNode> node =
        std::make_unique<ASTNode>();

    node->type = TypeNode::MOVE;

    node->children.push_back(
        std::move(variable)
    );

    node->children.push_back(
        std::move(expression)
    );

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newFunctionCall(
    std::unique_ptr<ASTNode> function)
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::FUNCTION_CALL;
    node->children.push_back(std::move(function));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newNULL()
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::null;
    node->as = static_cast<void*>(nullptr);

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newProgram()
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::PROGRAM;
    node->as = static_cast<void*>(nullptr);

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newBlock()
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::BLOCK;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newIf(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> trueBlock,
    std::unique_ptr<ASTNode> falseBlock)
{
    /*
    if cond {trueblock} else {falseblock}
    */

    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::IF;

    node->children.push_back(std::move(condition));
    node->children.push_back(std::move(trueBlock));

    if (falseBlock)
        node->children.push_back(std::move(falseBlock));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newWhile(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> block)
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::WHILE;

    node->children.push_back(std::move(condition));
    node->children.push_back(std::move(block));

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newImport(std::string_view name)
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::IMPORT;
    node->as = name;

    return node;
}

std::unique_ptr<ASTNode> ASTNode::ast_newMemberAccess(
    std::unique_ptr<ASTNode> object,
    std::unique_ptr<ASTNode> member)
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::MEMBER_ACCESS;

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

std::unique_ptr<ASTNode> ASTNode::ast_newFunctionDeclaration(
    std::unique_ptr<ASTNode> name,
    std::vector<FunctionParameter> parameters,
    std::unique_ptr<ASTNode> body)
{
    auto node = std::make_unique<ASTNode>();

    node->type = TypeNode::FUNCTION_DECLARATION;

    node->children.push_back(std::move(name));
    node->children.push_back(std::move(body));

    node->as =
        std::make_unique<std::vector<FunctionParameter>>(
            std::move(parameters)
        );

    return node;
}

const char* opToString(ASTNode::TypeOp op)
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
    for (int i = 0; i < indent; i++)
        std::cout << "    ";

    switch (type)
    {
    case TypeNode::INT:
        std::cout << "INT("
                  << std::get<int64_t>(as)
                  << ")";
        break;

    case TypeNode::FLOAT:
        std::cout << "FLOAT("
                  << std::get<double>(as)
                  << ")";
        break;

    case TypeNode::STRING:
        std::cout << "STRING("
                  << std::get<std::string_view>(as)
                  << ")";
        break;

    case TypeNode::BOOL:
        std::cout << "BOOL("
                  << std::get<bool>(as)
                  << ")";
        break;

    case TypeNode::VARIABLE:
        std::cout << "VARIABLE("
                  << std::get<std::string_view>(as)
                  << ")";
        break;

    case TypeNode::BINARY_EXPR:
        std::cout << "BINARY_EXPR op=" << op;
        break;

    case TypeNode::UNARY_EXPR:
        std::cout << "UNARY_EXPR op=" << op;
        break;

    case TypeNode::ASSIGNMENT:
        std::cout << "ASSIGNMENT";
        break;

    case TypeNode::FUNCTION_CALL:
        std::cout << "FUNCTION CALL";
        break;

    case TypeNode::PROGRAM:
        std::cout << "PROGRAM";
        break;

    case TypeNode::BLOCK:
        std::cout << "BLOCK";
        break;

    case TypeNode::WHILE:
        std::cout << "WHILE";
        break;

    case TypeNode::IMPORT:
        std::cout << "IMPORT("
                  << std::get<std::string_view>(as)
                  << ")";
        break;

    case TypeNode::MEMBER_ACCESS:
        std::cout << "MEMBER ACCESS";
        break;

    case TypeNode::VECTOR:
        std::cout << "VECTOR";
        break;

    case TypeNode::INDEX:
        std::cout << "INDEX";
        break;

    case TypeNode::IF:
        std::cout << "IF";
        break;

    case TypeNode::null:
        std::cout << "null";
        break;

    case TypeNode::CONTINUE:
        std::cout << "CONTINUE";
        break;

    case TypeNode::BREAK:
        std::cout << "BREAK";
        break;

    case TypeNode::RETURN:
        std::cout << "RETURN";
        break;

    case TypeNode::FUNCTION_DECLARATION:
    {
        std::cout << "FUNCTION DECLARATION";

        const auto& parameters =
            std::get<
                std::unique_ptr<std::vector<FunctionParameter>>
            >(as);

        if (parameters)
        {
            for (const auto& parameter : *parameters)
            {
                std::cout << "\n";

                for (int i = 0; i < indent + 1; i++)
                    std::cout << "    ";

                std::cout
                    << "PARAMETER("
                    << parameter.getName()
                    << ")"
                    << " type="
                    << parameter.getType()
                    << " const="
                    << parameter.isConst()
                    << " mutable="
                    << parameter.isMut();
            }
        }

        break;
    }

    case TypeNode::MOVE:
        std::cout << "MOVE";
        break;

    case TypeNode::CLASS_DECLARATION:
    {
        std::cout << "CLASS DECLARATION access=";

        switch (accessModifier)
        {
        case AccessModifier::PUBLIC:
            std::cout << "public";
            break;

        case AccessModifier::PRIVATE:
            std::cout << "private";
            break;

        case AccessModifier::PROTECTED:
            std::cout << "protected";
            break;
        }

        break;
    }

    case TypeNode::CLASS_FIELD_DECLARATION:
    {
        std::cout << "CLASS FIELD access=";

        switch (accessModifier)
        {
        case AccessModifier::PUBLIC:
            std::cout << "public";
            break;

        case AccessModifier::PRIVATE:
            std::cout << "private";
            break;

        case AccessModifier::PROTECTED:
            std::cout << "protected";
            break;

        default:
            std::cout << "unknown";
            break;
        }

        std::cout << " type=";

        switch (getTypeNode())
        {
        case TypeNode::INT:
            std::cout << "int";
            break;

        case TypeNode::FLOAT:
            std::cout << "float";
            break;

        case TypeNode::BOOL:
            std::cout << "bool";
            break;

        case TypeNode::STRING:
            std::cout << "string";
            break;

        case TypeNode::VECTOR:
            std::cout << "vector";
            break;

        case TypeNode::ANYVALUE:
            std::cout << "any";
            break;

        default:
            std::cout << "UNKNOWN";
            break;
        }

        break;
    }
    
    default:
        std::cout << "UNKNOWN";
        break;
    }

    std::cout << '\n';

    for (const auto& child : children)
    {
        if (child != nullptr)
            child->dump(indent + 1);
    }
}

std::unique_ptr<ASTNode> ASTNode::ast_clone(const ASTNode& node)
{
    auto copy = std::make_unique<ASTNode>();

    copy->type = node.type;
    copy->op = node.op;
    copy->accessModifier = node.accessModifier;

    if (std::holds_alternative<int64_t>(node.as))
    {
        copy->as = std::get<int64_t>(node.as);
    }
    else if (std::holds_alternative<double>(node.as))
    {
        copy->as = std::get<double>(node.as);
    }
    else if (std::holds_alternative<bool>(node.as))
    {
        copy->as = std::get<bool>(node.as);
    }
    else if (std::holds_alternative<void*>(node.as))
    {
        copy->as = std::get<void*>(node.as);
    }
    else if (std::holds_alternative<std::string_view>(node.as))
    {
        copy->as = std::get<std::string_view>(node.as);
    }
    else if (std::holds_alternative<TypeNode>(node.as))
    {
        copy->as = node.getTypeNode();
    }
    else if (
        std::holds_alternative<
            std::unique_ptr<std::vector<FunctionParameter>>
        >(node.as))
    {
        const auto& parameters =
            std::get<
                std::unique_ptr<std::vector<FunctionParameter>>
            >(node.as);

        if (parameters)
        {
            copy->as =
                std::make_unique<
                    std::vector<FunctionParameter>
                >(*parameters);
        }
        else
        {
            copy->as =
                std::unique_ptr<
                    std::vector<FunctionParameter>
                >{};
        }
    }

    for (const auto& child : node.children)
    {
        if (child != nullptr)
            copy->children.push_back(ast_clone(*child));
    }

    return copy;
}
