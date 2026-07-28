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
            return evalUni(node);
        case ASTNode::FUNCTION_CALL:
            return evalFuncCall(node);

        case ASTNode::PROGRAM: {
            RuntimeValue result(nullptr);
            for (const auto& stmt : node.children) {
                result = eval(*stmt);
            }
            return result;
        }

        case ASTNode::BLOCK:
            return evalBlock(node);

        case ASTNode::IF:
            return evalIf(node);

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
    
    //COMPARISONS

    case ASTNode::EQUAL:
        if (left.is<int64_t>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<int64_t>() == right.get<int64_t>()));
        }
        if (left.is<double>() && right.is<double>()) {
            return RuntimeValue((bool)(left.get<double>() == right.get<double>()));
        }
        if (left.is<bool>() && right.is<bool>()) {
            return RuntimeValue((bool)(left.get<bool>() == right.get<bool>()));
        }
        if (left.is<int64_t>() && right.is<double>()) {
            return RuntimeValue((bool)((double)left.get<int64_t>() == right.get<double>()));
        }
        if (left.is<double>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<double>() == (double)right.get<int64_t>()));
        }
        if (left.is<std::string>() && right.is<std::string>()) {
            return RuntimeValue((bool)(left.get<std::string>() == right.get<std::string>()));
        }
        return RuntimeValue((bool)false);
    case ASTNode::NOT_EQUAL:
        if (left.is<int64_t>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<int64_t>() != right.get<int64_t>()));
        }
        if (left.is<double>() && right.is<double>()) {
            return RuntimeValue((bool)(left.get<double>() != right.get<double>()));
        }
        if (left.is<bool>() && right.is<bool>()) {
            return RuntimeValue((bool)(left.get<bool>() != right.get<bool>()));
        }
        if (left.is<int64_t>() && right.is<double>()) {
            return RuntimeValue((bool)((double)left.get<int64_t>() != right.get<double>()));
        }
        if (left.is<double>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<double>() != (double)right.get<int64_t>()));
        }
        if (left.is<std::string>() && right.is<std::string>()) {
            return RuntimeValue((bool)(left.get<std::string>() != right.get<std::string>()));
        }
        return RuntimeValue((bool)true);
    case ASTNode::LESS:
        if (left.is<int64_t>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<int64_t>() < right.get<int64_t>()));
        }
        if (left.is<double>() && right.is<double>()) {
            return RuntimeValue((bool)(left.get<double>() < right.get<double>()));
        }
        if (left.is<bool>() && right.is<bool>()) {
            return RuntimeValue((bool)(left.get<bool>() < right.get<bool>()));
        }
        if (left.is<int64_t>() && right.is<double>()) {
            return RuntimeValue((bool)((double)left.get<int64_t>() < right.get<double>()));
        }
        if (left.is<double>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<double>() < (double)right.get<int64_t>()));
        }
        if (left.is<std::string>() && right.is<std::string>()) {
            return RuntimeValue((bool)(left.get<std::string>().size() < right.get<std::string>().size()));
        }
        return RuntimeValue((bool)false);
    case ASTNode::LESS_EQUAL:
        if (left.is<int64_t>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<int64_t>() <= right.get<int64_t>()));
        }
        if (left.is<double>() && right.is<double>()) {
            return RuntimeValue((bool)(left.get<double>() <= right.get<double>()));
        }
        if (left.is<bool>() && right.is<bool>()) {
            return RuntimeValue((bool)(left.get<bool>() <= right.get<bool>()));
        }
        if (left.is<int64_t>() && right.is<double>()) {
            return RuntimeValue((bool)((double)left.get<int64_t>() <= right.get<double>()));
        }
        if (left.is<double>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<double>() <= (double)right.get<int64_t>()));
        }
        if (left.is<std::string>() && right.is<std::string>()) {
            return RuntimeValue((bool)(left.get<std::string>().size() <= right.get<std::string>().size()));
        }
        return RuntimeValue((bool)false);
    case ASTNode::GREATER:
        if (left.is<int64_t>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<int64_t>() > right.get<int64_t>()));
        }
        if (left.is<double>() && right.is<double>()) {
            return RuntimeValue((bool)(left.get<double>() > right.get<double>()));
        }
        if (left.is<bool>() && right.is<bool>()) {
            return RuntimeValue((bool)(left.get<bool>() > right.get<bool>()));
        }
        if (left.is<int64_t>() && right.is<double>()) {
            return RuntimeValue((bool)((double)left.get<int64_t>() > right.get<double>()));
        }
        if (left.is<double>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<double>() > (double)right.get<int64_t>()));
        }
        if (left.is<std::string>() && right.is<std::string>()) {
            return RuntimeValue((bool)(left.get<std::string>().size() > right.get<std::string>().size()));
        }
        return RuntimeValue((bool)false);
    case ASTNode::GREATER_EQUAL:
        if (left.is<int64_t>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<int64_t>() >= right.get<int64_t>()));
        }
        if (left.is<double>() && right.is<double>()) {
            return RuntimeValue((bool)(left.get<double>() >= right.get<double>()));
        }
        if (left.is<bool>() && right.is<bool>()) {
            return RuntimeValue((bool)(left.get<bool>() >= right.get<bool>()));
        }
        if (left.is<int64_t>() && right.is<double>()) {
            return RuntimeValue((bool)((double)left.get<int64_t>() >= right.get<double>()));
        }
        if (left.is<double>() && right.is<int64_t>()) {
            return RuntimeValue((bool)(left.get<double>() >= (double)right.get<int64_t>()));
        }
        if (left.is<std::string>() && right.is<std::string>()) {
            return RuntimeValue((bool)(left.get<std::string>().size() >= right.get<std::string>().size()));
        }
        return RuntimeValue((bool)false);
    
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

RuntimeValue Interpreter::evalUni(const ASTNode& node) {

    RuntimeValue value = eval(*node.children[0]);

    switch (node.getOp()) {
        case ASTNode::NEGATIVE:
        {
            if (value.is<bool>())
                return RuntimeValue(!value.get<bool>());
            else if (value.is<int64_t>())
                return RuntimeValue(-(value.get<int64_t>()));
            else if (value.is<double>())
                return RuntimeValue(-(value.get<double>()));
            else
                return RuntimeValue(nullptr);
        }
    }
    return RuntimeValue(nullptr);

}

RuntimeValue Interpreter::evalBlock(const ASTNode& node) {
    runtime.pushEnvironment();

    RuntimeValue result(nullptr);

    for (const auto& child : node.children) {
        result = eval(*child);
    }

    runtime.popEnvironment();

    return result;
}

RuntimeValue Interpreter::evalIf(const ASTNode& node) {

    RuntimeValue cond = eval(*node.children[0]); //def-left

    /*
    caso nao seja bool caira sempre para o null pq todas as comparaçoes dao bool 
    */

    bool result = false; //deixar inicializado por padrao
    if (cond.is<bool>()) {
        result = cond.get<bool>();
    } else {
        return RuntimeValue(nullptr);
    }
    
    if (result) return eval(*node.children[1]);
    //se tiver else:
    if (node.children.size() == 3) return eval(*node.children[2]);

    return RuntimeValue(nullptr); //failsafe
}