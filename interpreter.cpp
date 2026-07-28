#include "runtime/runtimeval.hpp"
#include "runtime/runtime.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "builtin.hpp"
#include <math.h>

#define LEFTPARTOFTREE children[0]
#define RIGHTPARTOFTREE children[1]

// RuntimeValue Interpreter::eval(std::unique_ptr<ASTNode> node) {

//     ASTNode::TypeNode type = node.get()->getType();

//     switch (type)
//     {
//     case ASTNode::INT:
//         {
//             RuntimeValue rtval(node.get()->getInt());
//             return rtval;
//         }
//     case ASTNode::FLOAT:
//         {
//             RuntimeValue rtval(node.get()->getFloat());
//             return rtval;
//         }
//     case ASTNode::STRING:
//         {
//             std::string_view tempstr = node.get()->getString();
//             std::string str(tempstr);
//             RuntimeValue rtval(std::move(str));
//             return rtval;
//         }
//     case ASTNode::BOOL:
//         {
//             RuntimeValue rtval(node.get()->getBool());
//             return rtval;
//         }
//     case ASTNode::null:
//         {
//             return RuntimeValue(nullptr);
//         }
//     case ASTNode::BINARY_EXPR:
//         {
//             return evalBinary(std::move(node));
//         }
//     case ASTNode::VARIABLE:
//         {
//             return evalVar(std::move(node));
//         }
//     case ASTNode::ASSIGNMENT:
//         {
//             return evalAssignment(std::move(node));
//         }
//     case ASTNode::UNARY_EXPR:
//         {
//             return evalUni(std::move(node));
//         }
//     default:
//         break;
//     }
//     return RuntimeValue(nullptr);
// }

RuntimeValue Interpreter::eval(const ASTNode& node)
{
    if (&node == nullptr) return RuntimeValue(nullptr);
    switch(node.getType())
    {
        case ASTNode::INT:
            return RuntimeValue(node.getInt());

        case ASTNode::FLOAT:
            return RuntimeValue(node.getFloat());

        case ASTNode::STRING:
        {
            return RuntimeValue(std::string(node.getString()));
        }

        case ASTNode::BOOL:
            return RuntimeValue(node.getBool());

        case ASTNode::null:
            return RuntimeValue(nullptr);

        case ASTNode::BINARY_EXPR:
            return evalBinary(node);

        case ASTNode::VARIABLE:
            return evalVar(node);

        case ASTNode::ASSIGNMENT:
            return evalAssign(node);

        case ASTNode::UNARY_EXPR:
            //return evalUni(node);
        case ASTNode::FUNCTION_CALL:
            return evalFuncCall(node);

        case ASTNode::PROGRAM: {
            RuntimeValue result(nullptr);
            for (const auto& stmt : node.children) {
                result = eval(*stmt);
            }
            return result;
        }


        default:
            return RuntimeValue(nullptr);
    }
}

RuntimeValue Interpreter::evalBinary(const ASTNode& node) {
    auto left = eval(*node.LEFTPARTOFTREE);
    auto right = eval(*node.RIGHTPARTOFTREE);

    switch (node.getOp()) {
    case ASTNode::ADD:
        {if(left.is<int64_t>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<int64_t>() + right.get<int64_t>());
        }
        if(left.is<double>() && right.is<double>())
        {
            return RuntimeValue(left.get<double>() + right.get<double>());
        }
        if(left.is<int64_t>() && right.is<double>())
        {
            return RuntimeValue(left.get<int64_t>() + right.get<double>());
        }
        if(left.is<double>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<double>() + right.get<int64_t>());
        }
        if(left.is<std::string>() && right.is<std::string>())
        {
            return RuntimeValue(left.get<std::string>() + right.get<std::string>());
        }
        return RuntimeValue(nullptr);}
    case ASTNode::SUB:
        {if(left.is<int64_t>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<int64_t>() - right.get<int64_t>());
        }
        if(left.is<double>() && right.is<double>())
        {
            return RuntimeValue(left.get<double>() - right.get<double>());
        }
        if(left.is<int64_t>() && right.is<double>())
        {
            return RuntimeValue(left.get<int64_t>() - right.get<double>());
        }
        if(left.is<double>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<double>() - right.get<int64_t>());
        }
        return RuntimeValue(nullptr);}
    case ASTNode::MUL:
        {if(left.is<int64_t>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<int64_t>() * right.get<int64_t>());
        }
        if(left.is<double>() && right.is<double>())
        {
            return RuntimeValue(left.get<double>() * right.get<double>());
        }
        if(left.is<int64_t>() && right.is<double>())
        {
            return RuntimeValue(left.get<int64_t>() * right.get<double>());
        }
        if(left.is<double>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<double>() * right.get<int64_t>());
        }
        return RuntimeValue(nullptr);}
    case ASTNode::DIV:
        {if(left.is<int64_t>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<int64_t>() / right.get<int64_t>());
        }
        if(left.is<double>() && right.is<double>())
        {
            return RuntimeValue(left.get<double>() / right.get<double>());
        }
        if(left.is<int64_t>() && right.is<double>())
        {
            return RuntimeValue(left.get<int64_t>() / right.get<double>());
        }
        if(left.is<double>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<double>() / right.get<int64_t>());
        }
        return RuntimeValue(nullptr);
    case ASTNode::MOD:
        if(left.is<int64_t>() && right.is<int64_t>())
        {
            return RuntimeValue(left.get<int64_t>() % right.get<int64_t>());
        }
        return RuntimeValue(nullptr);}
    case ASTNode::POW:
        {double templeft, tempright;
        if (left.is<int64_t>()) {
            templeft = (double)left.get<int64_t>();
        } else if (left.is<double>()) {
            templeft = (double)left.get<double>();
        } else {
            return RuntimeValue(nullptr);
        }
        if (right.is<int64_t>()) {
            tempright = (double)right.get<int64_t>();
        } else if (right.is<double>()) {
            tempright = (double)right.get<double>();
        } else {
            return RuntimeValue(nullptr);
        }
        return RuntimeValue(pow(templeft,tempright));}
    case ASTNode::AND:
        {bool templeft, tempright;    
        if (left.is<bool>()) {
            templeft = left.get<bool>();
        } else if (left.is<int64_t>()) {
            templeft = (bool)left.get<int64_t>();
        } else if (left.is<double>()) {
            templeft = (bool)left.get<double>();
        } else return RuntimeValue(nullptr);
        if (right.is<bool>()) {
            tempright = right.get<bool>();
        } else if (right.is<int64_t>()) {
            tempright = (bool)right.get<int64_t>();
        } else if (right.is<double>()) {
            tempright = (bool)right.get<double>();
        } else return RuntimeValue(nullptr);
        return RuntimeValue((bool)(templeft && tempright));}
    case ASTNode::OR:
        {bool templeft, tempright;    
        if (left.is<bool>()) {
            templeft = left.get<bool>();
        } else if (left.is<int64_t>()) {
            templeft = (bool)left.get<int64_t>();
        } else if (left.is<double>()) {
            templeft = (bool)left.get<double>();
        } else return RuntimeValue(nullptr);
        if (right.is<bool>()) {
            tempright = right.get<bool>();
        } else if (right.is<int64_t>()) {
            tempright = (bool)right.get<int64_t>();
        } else if (right.is<double>()) {
            tempright = (bool)right.get<double>();
        } else return RuntimeValue(nullptr);
        return RuntimeValue((bool)(templeft || tempright));}

    default:
        return RuntimeValue(nullptr);
    }

    //failsafe
    return RuntimeValue(nullptr);
}

RuntimeValue Interpreter::evalAssign(const ASTNode& node) {
    auto value = eval(*node.RIGHTPARTOFTREE);
    if (!runtime.current->assignVar(std::string(node.LEFTPARTOFTREE->getString()), value)) {
        runtime.current->createVar(std::string(node.LEFTPARTOFTREE->getString()), value);
    }
    return value;
}

//dps ao implementar arrays tem que ser diferente
RuntimeValue Interpreter::evalVar(const ASTNode& node)
{
    std::string name(node.getString());

    RuntimeValue* value = runtime.current->findVar(name);

    if (value == nullptr) {
        //// TODO: gerar erro de variável inexistente
        return RuntimeValue(nullptr);
    }

    return *value;
}

RuntimeValue Interpreter::evalFuncCall(const ASTNode& node) {

    const ASTNode& function = *node.children[0];
    std::string name(function.getString());

    std::vector<RuntimeValue> args;
    args.reserve(node.children.size());
    for (size_t i = 1; i < node.children.size(); i++) {
        args.push_back(eval(*node.children[i]));
    }

    //builtins
    BuiltinFunc func = builtin::findFunction(name);
    std::cout << "Function name = '" << name << "'\n";

    if (func != nullptr) {return func(args);}

    //userfuncs.

    /*
    //inicio
    Environment local(runtime.current);
    runtime.current = &local;
    
    //codigo

    runtime.current = local.parent;
    //fim
    */

    return nullptr;
}