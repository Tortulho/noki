#include "runtime/runtimeval.hpp"
#include "runtime/runtime.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "builtin.hpp"
#include "runtime/runtimevalVector.hpp"
#include <math.h>

#define LEFTPARTOFTREE children[0]
#define RIGHTPARTOFTREE children[1]

RuntimeValue* Interpreter::resolveLValue(const ASTNode& node)
{
    if (node.getType() == ASTNode::VARIABLE)
    {
        return runtime.current->findVar(
            std::string(node.getString())
        );
    }

    if (node.getType() == ASTNode::INDEX)
    {
        if (node.children.size() != 2) return nullptr;

        RuntimeValue* object = resolveLValue(*node.children[0]);

        if (object == nullptr) return nullptr;

        RuntimeValue index = eval(*node.children[1]);

        if (!index.is<int64_t>()) return nullptr;

        if (!object->isVector()) return nullptr;

        auto* vector = object->getVector();

        if (vector == nullptr) return nullptr;

        int64_t indexValue = index.get<int64_t>();

        if (indexValue < 0) return nullptr;

        if (static_cast<size_t>(indexValue) >= vector->size()) return nullptr;

        return &vector->at(
            static_cast<size_t>(indexValue)
        );
    }

    return nullptr;
}

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
        case ASTNode::WHILE:
            return evalWhile(node);
        case ASTNode::MEMBER_ACCESS:
            return evalMemberAccess(node);
        case ASTNode::TypeNode::IMPORT:
            return evalImport(node);
        case ASTNode::VECTOR:
            return evalVector(node);
        case ASTNode::INDEX:
            return evalIndex(node);

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
        // if (left.is<int64_t>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<int64_t>() == right.get<int64_t>()));
        // }
        // if (left.is<double>() && right.is<double>()) {
        //     return RuntimeValue((bool)(left.get<double>() == right.get<double>()));
        // }
        // if (left.is<bool>() && right.is<bool>()) {
        //     return RuntimeValue((bool)(left.get<bool>() == right.get<bool>()));
        // }
        // if (left.is<int64_t>() && right.is<double>()) {
        //     return RuntimeValue((bool)((double)left.get<int64_t>() == right.get<double>()));
        // }
        // if (left.is<double>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<double>() == (double)right.get<int64_t>()));
        // }
        // if (left.is<std::string>() && right.is<std::string>()) {
        //     return RuntimeValue((bool)(left.get<std::string>() == right.get<std::string>()));
        // }
        // return RuntimeValue((bool)false);
    case ASTNode::NOT_EQUAL:
        // if (left.is<int64_t>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<int64_t>() != right.get<int64_t>()));
        // }
        // if (left.is<double>() && right.is<double>()) {
        //     return RuntimeValue((bool)(left.get<double>() != right.get<double>()));
        // }
        // if (left.is<bool>() && right.is<bool>()) {
        //     return RuntimeValue((bool)(left.get<bool>() != right.get<bool>()));
        // }
        // if (left.is<int64_t>() && right.is<double>()) {
        //     return RuntimeValue((bool)((double)left.get<int64_t>() != right.get<double>()));
        // }
        // if (left.is<double>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<double>() != (double)right.get<int64_t>()));
        // }
        // if (left.is<std::string>() && right.is<std::string>()) {
        //     return RuntimeValue((bool)(left.get<std::string>() != right.get<std::string>()));
        // }
        // return RuntimeValue((bool)true);
    case ASTNode::LESS:
        // if (left.is<int64_t>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<int64_t>() < right.get<int64_t>()));
        // }
        // if (left.is<double>() && right.is<double>()) {
        //     return RuntimeValue((bool)(left.get<double>() < right.get<double>()));
        // }
        // if (left.is<bool>() && right.is<bool>()) {
        //     return RuntimeValue((bool)(left.get<bool>() < right.get<bool>()));
        // }
        // if (left.is<int64_t>() && right.is<double>()) {
        //     return RuntimeValue((bool)((double)left.get<int64_t>() < right.get<double>()));
        // }
        // if (left.is<double>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<double>() < (double)right.get<int64_t>()));
        // }
        // if (left.is<std::string>() && right.is<std::string>()) {
        //     return RuntimeValue((bool)(left.get<std::string>().size() < right.get<std::string>().size()));
        // }
        // return RuntimeValue((bool)false);
    case ASTNode::LESS_EQUAL:
        // if (left.is<int64_t>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<int64_t>() <= right.get<int64_t>()));
        // }
        // if (left.is<double>() && right.is<double>()) {
        //     return RuntimeValue((bool)(left.get<double>() <= right.get<double>()));
        // }
        // if (left.is<bool>() && right.is<bool>()) {
        //     return RuntimeValue((bool)(left.get<bool>() <= right.get<bool>()));
        // }
        // if (left.is<int64_t>() && right.is<double>()) {
        //     return RuntimeValue((bool)((double)left.get<int64_t>() <= right.get<double>()));
        // }
        // if (left.is<double>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<double>() <= (double)right.get<int64_t>()));
        // }
        // if (left.is<std::string>() && right.is<std::string>()) {
        //     return RuntimeValue((bool)(left.get<std::string>().size() <= right.get<std::string>().size()));
        // }
        // return RuntimeValue((bool)false);
    case ASTNode::GREATER:
        // if (left.is<int64_t>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<int64_t>() > right.get<int64_t>()));
        // }
        // if (left.is<double>() && right.is<double>()) {
        //     return RuntimeValue((bool)(left.get<double>() > right.get<double>()));
        // }
        // if (left.is<bool>() && right.is<bool>()) {
        //     return RuntimeValue((bool)(left.get<bool>() > right.get<bool>()));
        // }
        // if (left.is<int64_t>() && right.is<double>()) {
        //     return RuntimeValue((bool)((double)left.get<int64_t>() > right.get<double>()));
        // }
        // if (left.is<double>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<double>() > (double)right.get<int64_t>()));
        // }
        // if (left.is<std::string>() && right.is<std::string>()) {
        //     return RuntimeValue((bool)(left.get<std::string>().size() > right.get<std::string>().size()));
        // }
        // return RuntimeValue((bool)false);
    case ASTNode::GREATER_EQUAL:
        // if (left.is<int64_t>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<int64_t>() >= right.get<int64_t>()));
        // }
        // if (left.is<double>() && right.is<double>()) {
        //     return RuntimeValue((bool)(left.get<double>() >= right.get<double>()));
        // }
        // if (left.is<bool>() && right.is<bool>()) {
        //     return RuntimeValue((bool)(left.get<bool>() >= right.get<bool>()));
        // }
        // if (left.is<int64_t>() && right.is<double>()) {
        //     return RuntimeValue((bool)((double)left.get<int64_t>() >= right.get<double>()));
        // }
        // if (left.is<double>() && right.is<int64_t>()) {
        //     return RuntimeValue((bool)(left.get<double>() >= (double)right.get<int64_t>()));
        // }
        // if (left.is<std::string>() && right.is<std::string>()) {
        //     return RuntimeValue((bool)(left.get<std::string>().size() >= right.get<std::string>().size()));
        // }
        // return RuntimeValue((bool)false);
        return RuntimeValue(bool(RuntimeValue::comp(left,right,node.getOp())));

    default:
        return RuntimeValue(nullptr);
    }

    //failsafe
    return RuntimeValue(nullptr);
}

// RuntimeValue Interpreter::evalAssign(const ASTNode& node) {
//     auto value = eval(*node.RIGHTPARTOFTREE);
//     if (!runtime.current->assignVar(std::string(node.LEFTPARTOFTREE->getString()), value)) {
//         runtime.current->createVar(std::string(node.LEFTPARTOFTREE->getString()), value);
//     }
//     return value;
// }

RuntimeValue Interpreter::evalAssign(const ASTNode& node)
{
    auto value = eval(*node.RIGHTPARTOFTREE);

    const ASTNode& target = *node.LEFTPARTOFTREE;

    if (target.getType() == ASTNode::VARIABLE)
    {
        if (!runtime.current->assignVar(
            std::string(target.getString()),
            value))
        {
            runtime.current->createVar(
                std::string(target.getString()),
                value
            );
        }

        return value;
    }

    if (target.getType() == ASTNode::INDEX)
    {
        RuntimeValue* targetValue = resolveLValue(target);

        if (targetValue == nullptr)
            return RuntimeValue(nullptr);

        *targetValue = std::move(value);

        return *targetValue;
    }

    return RuntimeValue(nullptr);
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

// RuntimeValue Interpreter::evalFuncCall(const ASTNode& node) {

//     const ASTNode& function = *node.children[0];
//     std::string name(function.getString());

//     std::vector<RuntimeValue> args;
//     args.reserve(node.children.size());
//     for (size_t i = 1; i < node.children.size(); i++) {
//         args.push_back(eval(*node.children[i]));
//     }

//     //builtins
//     BuiltinFunc func = builtin::findFunction(name);
//     std::cout << "Function name = '" << name << "'\n";

//     if (func != nullptr) {return func(args);}

//     //userfuncs.

//     /*
//     //inicio
//     Environment local(runtime.current);
//     runtime.current = &local;
    
//     //codigo

//     runtime.current = local.parent;
//     //fim
//     */

//     return nullptr;
// }

RuntimeValue Interpreter::evalFuncCall(const ASTNode& node)
{
    if (node.children.empty()) return RuntimeValue(nullptr);

    const ASTNode& target = *node.children[0];

    // função normal/mutável do Noki
    if (target.getType() == ASTNode::VARIABLE)
    {
        std::string name(target.getString());

        const BuiltinEntry* entry =
            builtin::findFunction(name);

        if (entry != nullptr)
        {
            return std::visit(
                [&](const auto& func) -> RuntimeValue
                {
                    using T = std::decay_t<decltype(func)>;

                    if constexpr (
                        std::is_same_v<T, BuiltinFunc>
                    )
                    {
                        std::vector<RuntimeValue> args;
                        args.reserve(node.children.size() - 1);

                        for (size_t i = 1;
                             i < node.children.size();
                             ++i)
                        {
                            args.push_back(
                                eval(*node.children[i])
                            );
                        }

                        return func(args);
                    }
                    else if constexpr (
                        std::is_same_v<T, BuiltinMutFunc>
                    )
                    {
                        std::vector<RuntimeValue> temporaries;
                        std::vector<RuntimeValue*> args;

                        temporaries.reserve(
                            node.children.size() - 1
                        );

                        args.reserve(
                            node.children.size() - 1
                        );

                        for (size_t i = 1;
                             i < node.children.size();
                             ++i)
                        {
                            RuntimeValue* reference =
                                resolveLValue(
                                    *node.children[i]
                                );

                            if (reference != nullptr)
                            {
                                args.push_back(reference);
                            }
                            else
                            {
                                temporaries.push_back(
                                    eval(*node.children[i])
                                );

                                args.push_back(
                                    &temporaries.back()
                                );
                            }
                        }

                        return func(args);
                    }
                },
                entry->function
            );
        }
    }

    // função pertencente a uma biblioteca
    if (target.getType() == ASTNode::MEMBER_ACCESS)
{
    if (target.children.size() != 2)
        return RuntimeValue(nullptr);

    const ASTNode& objectNode = *target.children[0];
    const ASTNode& memberNode = *target.children[1];

    if (memberNode.getType() != ASTNode::VARIABLE)
        return RuntimeValue(nullptr);

    std::string functionName(memberNode.getString());

    /*
     * Primeiro tentamos tratar como método builtin.
     */
    const BuiltinEntry* entry =
        builtin::findFunction(functionName);

    if (entry != nullptr)
    {
        return std::visit(
            [&](const auto& func) -> RuntimeValue
            {
                using T = std::decay_t<decltype(func)>;

                /*
                 * Método builtin normal:
                 *
                 * object é avaliado como um valor normal.
                 */
                if constexpr (
                    std::is_same_v<T, BuiltinFunc>
                )
                {
                    std::vector<RuntimeValue> args;
                    args.reserve(node.children.size());

                    args.push_back(eval(objectNode));

                    for (size_t i = 1;
                         i < node.children.size();
                         ++i)
                    {
                        args.push_back(
                            eval(*node.children[i])
                        );
                    }

                    return func(args);
                }

                /*
                 * Método builtin mutável:
                 *
                 * object tem de ser um l-value.
                 */
                else if constexpr (
                    std::is_same_v<T, BuiltinMutFunc>
                )
                {
                    std::vector<RuntimeValue> temporaries;
                    std::vector<RuntimeValue*> args;

                    /*
                     * +1 porque o receiver é o primeiro argumento.
                     */
                    temporaries.reserve(
                        node.children.size() - 1
                    );

                    args.reserve(
                        node.children.size()
                    );

                    /*
                     * Receiver: x
                     */
                    RuntimeValue* receiver =
                        resolveLValue(objectNode);

                    if (receiver == nullptr)
                        return RuntimeValue(nullptr);

                    args.push_back(receiver);

                    /*
                     * Restantes argumentos.
                     */
                    for (size_t i = 1;
                         i < node.children.size();
                         ++i)
                    {
                        RuntimeValue* reference =
                            resolveLValue(
                                *node.children[i]
                            );

                        if (reference != nullptr)
                        {
                            args.push_back(reference);
                        }
                        else
                        {
                            temporaries.push_back(
                                eval(*node.children[i])
                            );

                            args.push_back(
                                &temporaries.back()
                            );
                        }
                    }

                    return func(args);
                }
            },
            entry->function
        );
        }

        /*
        * Não era builtin:
        * continuar com o mecanismo atual de biblioteca.
        */
        const ASTNode& libraryNode = objectNode;

        if (libraryNode.getType() != ASTNode::VARIABLE)
            return RuntimeValue(nullptr);

        std::string libraryName(
            libraryNode.getString()
        );

        Library* library =
            runtime.libraries.findLibrary(libraryName);

        if (library == nullptr)
            return RuntimeValue(nullptr);

        if (!runtime.libraries.isImported(libraryName))
            return RuntimeValue(nullptr);

        auto it = library->functions.find(functionName);

        if (it == library->functions.end())
            return RuntimeValue(nullptr);

        std::vector<RuntimeValue> args;
        args.reserve(node.children.size() - 1);

        for (size_t i = 1;
            i < node.children.size();
            ++i)
        {
            args.push_back(
                eval(*node.children[i])
            );
        }

        return it->second(args);
        }

    return RuntimeValue(nullptr);
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

RuntimeValue Interpreter::evalWhile(const ASTNode& node)
{
    RuntimeValue result(nullptr);

    while (RuntimeValue::runtimeValueToBool(eval(*node.children[0])))
    {
        result = eval(*node.children[1]);
    }

    return result;
}

//TODO COMPLETE
RuntimeValue Interpreter::evalImport(const ASTNode& node)
{
    const std::string name(node.getString());

    if (!runtime.libraries.importLibrary(name))
    {
        throw std::runtime_error(
            "Library not found: " + name
        );
    }

    return RuntimeValue(nullptr);
}

RuntimeValue Interpreter::evalMemberAccess(const ASTNode& node)
{
    if (node.children.size() != 2) return RuntimeValue(nullptr);

    const ASTNode& object = *node.children[0];
    const ASTNode& member = *node.children[1];

    if (object.getType() != ASTNode::VARIABLE) return RuntimeValue(nullptr);

    if (member.getType() != ASTNode::VARIABLE) return RuntimeValue(nullptr);

    std::string libraryName(object.getString());
    std::string memberName(member.getString());

    Library* library = runtime.libraries.findLibrary(libraryName);

    if (library == nullptr) return RuntimeValue(nullptr);

    auto it = library->functions.find(memberName);

    if (it == library->functions.end()) return RuntimeValue(nullptr);

    return RuntimeValue(reinterpret_cast<void*>(it->second));
}

RuntimeValue Interpreter::evalVector(const ASTNode& node)
{
    auto vector = std::make_unique<RuntimeValue_Vector>();

    for (const auto& child : node.children)
    {
        RuntimeValue value = eval(*child);

        vector->pushback(std::move(value));
    }

    return RuntimeValue(std::move(vector));
}

RuntimeValue Interpreter::evalIndex(const ASTNode& node)
{
    if (node.children.size() != 2) return RuntimeValue(nullptr);

    RuntimeValue object = eval(*node.children[0]);
    RuntimeValue index = eval(*node.children[1]);

    if (!object.isVector()) return RuntimeValue(nullptr);

    if (!index.is<int64_t>()) return RuntimeValue(nullptr);

    auto* vector = object.getVector();

    if (vector == nullptr) return RuntimeValue(nullptr);

    int64_t indexValue = index.get<int64_t>();

    if (indexValue < 0) return RuntimeValue(nullptr);

    if (static_cast<size_t>(indexValue) >= vector->size())
        return RuntimeValue(nullptr);

    return vector->at(static_cast<size_t>(indexValue));
}