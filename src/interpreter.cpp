#include "runtimeval.hpp"
#include "runtime.hpp"
#include "ast.hpp"
#include "asttypes.hpp"
#include "interpreter.hpp"
#include "builtin.hpp"
#include "builtinEntry.hpp"   
#include "runtimevalVector.hpp"
#include <math.h>

using TypeNode = ::TypeNode;

#define LEFTPARTOFTREE children[0]
#define RIGHTPARTOFTREE children[1]

RuntimeValue* Interpreter::resolveLValue(const ASTNode& node)
{
    if (node.getType() == TypeNode::VARIABLE)
    {
        return runtime.current->findVar(
            std::string(node.getString())
        );
    }

    if (node.getType() == TypeNode::INDEX)
    {
        if (node.children.size() != 2) return nullptr;

        RuntimeValue* object = resolveLValue(*node.children[0]);

        if (object == nullptr) return nullptr;

        EvalResult indexResult = eval(*node.children[1]);

        if (indexResult.control != FlowControl::NONE)
        {
            throw std::runtime_error(
                "Invalid control flow inside index expression."
            );
        }

        RuntimeValue index = std::move(indexResult.value);

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

EvalResult Interpreter::eval(const ASTNode& node)
{
    if (&node == nullptr) return {RuntimeValue(nullptr),FlowControl::INVALID}; //FAILSAFE
    switch(node.getType())
    {
        case TypeNode::INT:
            return {RuntimeValue(node.getInt()),FlowControl::NONE};

        case TypeNode::FLOAT:
            return {RuntimeValue(node.getFloat()),FlowControl::NONE};

        case TypeNode::STRING:
        {
            //simple string formating escape chars
            std::string value;
            std::string_view str = node.getString();

            for (size_t i = 0; i < str.size(); ++i) {
                if (str[i] != '\\') {
                    value += str[i];
                    continue;
                }
                ++i;
                if (i >= str.size()) break;

                switch (str[i])
                {
                    case 'n':
                        value += '\n';
                        break;
                    case 't':
                        value += '\t';
                        break;
                    case 'b':
                        if (!value.empty()) value.pop_back();
                        break;
                    case 'r':
                        value += '\r';
                        break;
                    case '\\':
                        value += '\\';
                        break;
                    case '"':
                        value += '"';
                        break;

                    default:
                        value += str[i];
                        break;
                }
            }
            return {RuntimeValue(std::move(value)),FlowControl::NONE};
        }

        case TypeNode::BOOL:
            return {RuntimeValue(node.getBool()),FlowControl::NONE};
        case TypeNode::null:
            return {RuntimeValue(nullptr),FlowControl::NONE};
        case TypeNode::BINARY_EXPR:
            return evalBinary(node);
        case TypeNode::VARIABLE:
            return evalVar(node);
        case TypeNode::ASSIGNMENT:
            return evalAssign(node);
        case TypeNode::UNARY_EXPR:
            return evalUni(node);
        case TypeNode::FUNCTION_CALL:
            return evalFuncCall(node);

        case TypeNode::PROGRAM:
        {
            EvalResult result{
                RuntimeValue(nullptr),
                FlowControl::NONE
            };
            for (const auto& stmt : node.children) {
                result = eval(*stmt);
                if (result.control != FlowControl::NONE)
                    return result;
            }
            return result;
        }

        case TypeNode::BLOCK:
            return evalBlock(node);
        case TypeNode::IF:
            return evalIf(node);
        case TypeNode::WHILE:
            return evalWhile(node);
        case TypeNode::MEMBER_ACCESS:
            return evalMemberAccess(node);
        case TypeNode::IMPORT:
            return evalImport(node);
        case TypeNode::VECTOR:
            return evalVector(node);
        case TypeNode::INDEX:
            return evalIndex(node);
        case TypeNode::FUNCTION_DECLARATION:
            return evalFuncDecl(node);
        case TypeNode::MOVE:
            return evalMove(node);

        case TypeNode::BREAK:
            return {
                RuntimeValue(nullptr),
                FlowControl::BREAK
            };

        case TypeNode::CONTINUE:
            return {
                RuntimeValue(nullptr),
                FlowControl::CONTINUE
            };

        case TypeNode::RETURN:
        {
            if (node.children.empty()) {
                return {
                    RuntimeValue(nullptr),
                    FlowControl::RETURN
                };
            }

            EvalResult result = eval(*node.children[0]);

            if (result.control != FlowControl::NONE)
                return result;

            return {
                std::move(result.value),
                FlowControl::RETURN
            };
        }

        case TypeNode::EXIT:
            std::exit(EXIT_SUCCESS);

        default:
            return {RuntimeValue(nullptr),FlowControl::NONE};
    }
}

EvalResult Interpreter::evalBinary(const ASTNode& node) {
    auto leftresult = eval(*node.LEFTPARTOFTREE);
    auto rightresult = eval(*node.RIGHTPARTOFTREE);

    if (leftresult.control != FlowControl::NONE)
    throw std::runtime_error(
        "Invalid control flow inside binary expression."
    );
    if (rightresult.control != FlowControl::NONE)
    throw std::runtime_error(
        "Invalid control flow inside binary expression."
    );

    RuntimeValue left = std::move(leftresult.value);
    RuntimeValue right = std::move(rightresult.value);

    switch (node.getOp()) {
    case ASTNode::ADD:
        {if(left.is<int64_t>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<int64_t>() + right.get<int64_t>()),FlowControl::NONE};
        }
        if(left.is<double>() && right.is<double>())
        {
            return {RuntimeValue(left.get<double>() + right.get<double>()),FlowControl::NONE};
        }
        if(left.is<int64_t>() && right.is<double>())
        {
            return {RuntimeValue(left.get<int64_t>() + right.get<double>()),FlowControl::NONE};
        }
        if(left.is<double>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<double>() + right.get<int64_t>()),FlowControl::NONE};
        }
        if(left.is<std::string>() && right.is<std::string>())
        {
            return {RuntimeValue(left.get<std::string>() + right.get<std::string>()),FlowControl::NONE};
        }
        return {RuntimeValue(nullptr),FlowControl::NONE};}
    case ASTNode::SUB:
        {if(left.is<int64_t>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<int64_t>() - right.get<int64_t>()),FlowControl::NONE};
        }
        if(left.is<double>() && right.is<double>())
        {
            return {RuntimeValue(left.get<double>() - right.get<double>()),FlowControl::NONE};
        }
        if(left.is<int64_t>() && right.is<double>())
        {
            return {RuntimeValue(left.get<int64_t>() - right.get<double>()),FlowControl::NONE};
        }
        if(left.is<double>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<double>() - right.get<int64_t>()),FlowControl::NONE};
        }
        return {RuntimeValue(nullptr),FlowControl::NONE};}
    case ASTNode::MUL:
        {if(left.is<int64_t>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<int64_t>() * right.get<int64_t>()),FlowControl::NONE};
        }
        if(left.is<double>() && right.is<double>())
        {
            return {RuntimeValue(left.get<double>() * right.get<double>()),FlowControl::NONE};
        }
        if(left.is<int64_t>() && right.is<double>())
        {
            return {RuntimeValue(left.get<int64_t>() * right.get<double>()),FlowControl::NONE};
        }
        if(left.is<double>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<double>() * right.get<int64_t>()),FlowControl::NONE};
        }
        return {RuntimeValue(nullptr),FlowControl::NONE};}
    case ASTNode::DIV:
        {if(left.is<int64_t>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<int64_t>() / right.get<int64_t>()),FlowControl::NONE};
        }
        if(left.is<double>() && right.is<double>())
        {
            return {RuntimeValue(left.get<double>() / right.get<double>()),FlowControl::NONE};
        }
        if(left.is<int64_t>() && right.is<double>())
        {
            return {RuntimeValue(left.get<int64_t>() / right.get<double>()),FlowControl::NONE};
        }
        if(left.is<double>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<double>() / right.get<int64_t>()),FlowControl::NONE};
        }
        return {RuntimeValue(nullptr),FlowControl::NONE};
    case ASTNode::MOD:
        if(left.is<int64_t>() && right.is<int64_t>())
        {
            return {RuntimeValue(left.get<int64_t>() % right.get<int64_t>()),FlowControl::NONE};
        }
        return {RuntimeValue(nullptr),FlowControl::NONE};}
    case ASTNode::POW:
        {double templeft, tempright;
        if (left.is<int64_t>()) {
            templeft = (double)left.get<int64_t>();
        } else if (left.is<double>()) {
            templeft = (double)left.get<double>();
        } else {
            return {RuntimeValue(nullptr),FlowControl::NONE};
        }
        if (right.is<int64_t>()) {
            tempright = (double)right.get<int64_t>();
        } else if (right.is<double>()) {
            tempright = (double)right.get<double>();
        } else {
            return {RuntimeValue(nullptr),FlowControl::NONE};
        }
        return {RuntimeValue(pow(templeft,tempright)),FlowControl::NONE};}
    case ASTNode::AND:
        {bool templeft, tempright;    
        if (left.is<bool>()) {
            templeft = left.get<bool>();
        } else if (left.is<int64_t>()) {
            templeft = (bool)left.get<int64_t>();
        } else if (left.is<double>()) {
            templeft = (bool)left.get<double>();
        } else return {RuntimeValue(nullptr),FlowControl::NONE};
        if (right.is<bool>()) {
            tempright = right.get<bool>();
        } else if (right.is<int64_t>()) {
            tempright = (bool)right.get<int64_t>();
        } else if (right.is<double>()) {
            tempright = (bool)right.get<double>();
        } else return {RuntimeValue(nullptr),FlowControl::NONE};
        return {RuntimeValue((bool)(templeft && tempright)),FlowControl::NONE};}
    case ASTNode::OR:
        {bool templeft, tempright;    
        if (left.is<bool>()) {
            templeft = left.get<bool>();
        } else if (left.is<int64_t>()) {
            templeft = (bool)left.get<int64_t>();
        } else if (left.is<double>()) {
            templeft = (bool)left.get<double>();
        } else return {RuntimeValue(nullptr),FlowControl::NONE};
        if (right.is<bool>()) {
            tempright = right.get<bool>();
        } else if (right.is<int64_t>()) {
            tempright = (bool)right.get<int64_t>();
        } else if (right.is<double>()) {
            tempright = (bool)right.get<double>();
        } else return {RuntimeValue(nullptr),FlowControl::NONE};
        return {RuntimeValue((bool)(templeft || tempright)),FlowControl::NONE};}
    
    //COMPARISONS
    case ASTNode::EQUAL:
    case ASTNode::NOT_EQUAL:
    case ASTNode::LESS:
    case ASTNode::LESS_EQUAL:
    case ASTNode::GREATER:
    case ASTNode::GREATER_EQUAL:
        return {RuntimeValue(bool(RuntimeValue::comp(left,right,node.getOp()))),FlowControl::NONE};

    default:
        return {RuntimeValue(nullptr),FlowControl::NONE};
    }

    //failsafe
    return {RuntimeValue(nullptr),FlowControl::NONE};
}

EvalResult Interpreter::evalAssign(const ASTNode& node)
{
    auto resultvalue = eval(*node.RIGHTPARTOFTREE);
    if (resultvalue.control != FlowControl::NONE)
        throw std::runtime_error(
            "Invalid control flow inside assignment expression."
        );
    RuntimeValue value = std::move(resultvalue.value);

    const ASTNode& target = *node.LEFTPARTOFTREE;

    if (target.getType() == TypeNode::VARIABLE)
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

        return {value,FlowControl::NONE};
    }

    if (target.getType() == TypeNode::INDEX)
    {
        RuntimeValue* targetValue = resolveLValue(target);

        if (targetValue == nullptr)
            return {RuntimeValue(nullptr),FlowControl::NONE};

        *targetValue = std::move(value);

        return {*targetValue,FlowControl::NONE};
    }

    return {RuntimeValue(nullptr),FlowControl::NONE};
}

//dps ao implementar arrays tem que ser diferente
EvalResult Interpreter::evalVar(const ASTNode& node)
{
    std::string name(node.getString());

    RuntimeValue* value = runtime.current->findVar(name);

    if (value == nullptr) {
        //// TODO: gerar erro de variável inexistente
        throw std::runtime_error(
            "Invalid or inexistent variable: '" + name + "'"
        );
    }

    return {*value,FlowControl::NONE};
}

EvalResult Interpreter::evalFuncCall(const ASTNode& node)
{
    if (node.children.empty())
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    const ASTNode& target = *node.children[0];

    /*
     * Função normal/mutável do Noki
     */
    if (target.getType() == TypeNode::VARIABLE)
    {
        std::string name(target.getString());

        /*
         * Primeiro tentamos a resolução C++ existente.
         */
        const BuiltinEntry* entry =
            builtin::findFunction(name);

        if (entry != nullptr)
        {
            RuntimeValue result =
                std::visit(
                    [&](const auto& func) -> RuntimeValue
                    {
                        using T =
                            std::decay_t<decltype(func)>;

                        if constexpr (
                            std::is_same_v<T, BuiltinFunc>
                        )
                        {
                            std::vector<RuntimeValue> args;

                            args.reserve(
                                node.children.size() - 1
                            );

                            for (size_t i = 1;
                                 i < node.children.size();
                                 ++i)
                            {
                                EvalResult argument =
                                    eval(*node.children[i]);

                                if (
                                    argument.control !=
                                    FlowControl::NONE
                                )
                                {
                                    throw std::runtime_error(
                                        "Invalid control flow in "
                                        "function argument."
                                    );
                                }

                                args.push_back(
                                    std::move(argument.value)
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
                                    args.push_back(
                                        reference
                                    );
                                }
                                else
                                {
                                    EvalResult argument =
                                        eval(*node.children[i]);

                                    if (
                                        argument.control !=
                                        FlowControl::NONE
                                    )
                                    {
                                        throw std::runtime_error(
                                            "Invalid control flow "
                                            "in function argument."
                                        );
                                    }

                                    temporaries.push_back(
                                        std::move(
                                            argument.value
                                        )
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

            return {
                std::move(result),
                FlowControl::NONE
            };
        }

        /*
         * Depois tentamos uma função Noki local.
         */
        NokiFunction* nokiFunction =
            runtime.current->findNokiFunction(name);

        if (nokiFunction != nullptr)
        {
            const auto& parameters =
                nokiFunction->getParameters();

            const size_t argumentCount =
                node.children.size() - 1;

            if (argumentCount != parameters.size())
            {
                throw std::runtime_error(
                    "Noki function '" +
                    name +
                    "' expected " +
                    std::to_string(parameters.size()) +
                    " arguments, got " +
                    std::to_string(argumentCount) +
                    "."
                );
            }

            /*
             * Avaliar e/ou resolver os argumentos antes de
             * criar o environment da função.
             */
            std::vector<RuntimeValue> argumentValues;

            argumentValues.resize(argumentCount);

            std::vector<RuntimeValue*> argumentReferences;

            argumentReferences.resize(
                argumentCount,
                nullptr
            );

            for (size_t i = 0;
                 i < parameters.size();
                 ++i)
            {
                const FunctionParameter& parameter =
                    parameters[i];

                const ASTNode& argumentNode =
                    *node.children[i + 1];

                if (
                    parameter.getMode() ==
                    FunctionParameterMode::MUT
                )
                {
                    RuntimeValue* reference =
                        resolveLValue(argumentNode);

                    if (reference == nullptr)
                    {
                        throw std::runtime_error(
                            "Noki function '" +
                            name +
                            "' parameter '" +
                            parameter.getName() +
                            "' requires a mutable lvalue."
                        );
                    }

                    argumentReferences[i] =
                        reference;
                }
                else
                {
                    EvalResult argument =
                        eval(argumentNode);

                    if (
                        argument.control !=
                        FlowControl::NONE
                    )
                    {
                        throw std::runtime_error(
                            "Invalid control flow in "
                            "function argument."
                        );
                    }

                    argumentValues[i] =
                        std::move(argument.value);
                }
            }

            /*
             * Validar os tipos dos parâmetros.
             */
            for (size_t i = 0;
                 i < parameters.size();
                 ++i)
            {
                ASTNode::TypeNode expectedType =
                    parameters[i].getType();

                bool valid = false;

                RuntimeValue* value = nullptr;

                if (
                    parameters[i].getMode() ==
                    FunctionParameterMode::MUT
                )
                {
                    value = argumentReferences[i];
                }
                else
                {
                    value = &argumentValues[i];
                }

                switch (expectedType)
                {
                    case TypeNode::ANYVALUE:
                        valid = true;
                        break;

                    case TypeNode::INT:
                        valid = value->is<int64_t>();
                        break;

                    case TypeNode::FLOAT:
                        valid = value->is<double>();
                        break;

                    case TypeNode::BOOL:
                        valid = value->is<bool>();
                        break;

                    case TypeNode::STRING:
                        valid = value->is<std::string>();
                        break;

                    case TypeNode::VECTOR:
                        valid = value->isVector();
                        break;

                    default:
                        valid = false;
                        break;
                }

                if (!valid)
                {
                    std::string expectedTypeName;

                    switch (expectedType)
                    {
                        case TypeNode::ANYVALUE:
                            expectedTypeName = "any";
                            break;

                        case TypeNode::INT:
                            expectedTypeName = "int";
                            break;

                        case TypeNode::FLOAT:
                            expectedTypeName = "float";
                            break;

                        case TypeNode::BOOL:
                            expectedTypeName = "bool";
                            break;

                        case TypeNode::STRING:
                            expectedTypeName = "string";
                            break;

                        case TypeNode::VECTOR:
                            expectedTypeName = "vector";
                            break;

                        default:
                            expectedTypeName = "unknown";
                            break;
                    }

                    throw std::runtime_error(
                        "Noki function '" +
                        name +
                        "' parameter '" +
                        parameters[i].getName() +
                        "' expected type " +
                        expectedTypeName +
                        "."
                    );
                }
            }

            /*
             * Criar o environment da chamada.
             */
            runtime.pushEnvironment();

            /*
             * Fazer o binding dos parâmetros.
             */
            for (size_t i = 0;
                 i < parameters.size();
                 ++i)
            {
                const FunctionParameter& parameter =
                    parameters[i];

                if (
                    parameter.getMode() ==
                    FunctionParameterMode::MUT
                )
                {
                    runtime.current->createReference(
                        parameter.getName(),
                        argumentReferences[i]
                    );
                }
                else if (
                    parameter.getMode() ==
                    FunctionParameterMode::CONST
                )
                {
                    runtime.current->createConst(
                        parameter.getName(),
                        std::move(argumentValues[i])
                    );
                }
                else
                {
                    runtime.current->createVar(
                        parameter.getName(),
                        std::move(argumentValues[i])
                    );
                }
            }

            /*
             * Executar o body da função.
             *
             * evalBlock() cria o seu próprio scope,
             * mantendo o environment da função como parent.
             */
            EvalResult result =
                eval(nokiFunction->getBody());

            runtime.popEnvironment();

            /*
             * RETURN pertence à função e é consumido aqui.
             */
            if (result.control == FlowControl::RETURN)
            {
                return {
                    std::move(result.value),
                    FlowControl::NONE
                };
            }

            /*
             * BREAK/CONTINUE não podem escapar de uma função.
             * Devem ter sido consumidos por um while.
             */
            if (
                result.control == FlowControl::BREAK ||
                result.control == FlowControl::CONTINUE
            )
            {
                throw std::runtime_error(
                    "Invalid break or continue outside "
                    "of a while loop."
                );
            }

            /*
             * Função terminou sem return explícito.
             */
            return {
                RuntimeValue(nullptr),
                FlowControl::NONE
            };
        }
    }

    /*
     * Função pertencente a uma biblioteca,
     * método de RuntimeObject ou método de RuntimeValue.
     */
    if (target.getType() == TypeNode::MEMBER_ACCESS)
    {
        if (target.children.size() != 2)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::NONE
            };
        }

        const ASTNode& objectNode =
            *target.children[0];

        const ASTNode& memberNode =
            *target.children[1];

        if (memberNode.getType() != TypeNode::VARIABLE)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::NONE
            };
        }

        const std::string functionName(
            memberNode.getString()
        );

        /*
         * Primeiro verificamos se o receiver é uma Library Noki.
         */
        if (objectNode.getType() == TypeNode::VARIABLE)
        {
            const std::string libraryName(
                objectNode.getString()
            );

            Library* library =
                runtime.libraries.findLibrary(
                    libraryName
                );

            if (library != nullptr)
            {
                auto moduleIt =
                    library->nokiModules.find(
                        libraryName
                    );

                if (moduleIt != library->nokiModules.end())
                {
                    NokiModule* module =
                        moduleIt->second.get();

                    if (module == nullptr)
                    {
                        return {
                            RuntimeValue(nullptr),
                            FlowControl::NONE
                        };
                    }

                    Environment* moduleEnvironment =
                        module->getEnvironment();

                    if (moduleEnvironment == nullptr)
                    {
                        throw std::runtime_error(
                            "Noki library '" +
                            libraryName +
                            "' has no environment."
                        );
                    }

                    NokiFunction* nokiFunction =
                        moduleEnvironment->findNokiFunction(
                            functionName
                        );

                    if (nokiFunction != nullptr)
                    {
                        const auto& parameters =
                            nokiFunction->getParameters();

                        const size_t argumentCount =
                            node.children.size() - 1;

                        if (
                            argumentCount !=
                            parameters.size()
                        )
                        {
                            throw std::runtime_error(
                                "Noki function '" +
                                functionName +
                                "' expected " +
                                std::to_string(
                                    parameters.size()
                                ) +
                                " arguments, got " +
                                std::to_string(
                                    argumentCount
                                ) +
                                "."
                            );
                        }

                        /*
                         * Os argumentos pertencem ao ambiente do caller.
                         */
                        std::vector<RuntimeValue> argumentValues;

                        argumentValues.resize(
                            argumentCount
                        );

                        std::vector<RuntimeValue*>
                            argumentReferences;

                        argumentReferences.resize(
                            argumentCount,
                            nullptr
                        );

                        for (size_t i = 0;
                             i < parameters.size();
                             ++i)
                        {
                            const FunctionParameter& parameter =
                                parameters[i];

                            const ASTNode& argumentNode =
                                *node.children[i + 1];

                            if (
                                parameter.getMode() ==
                                FunctionParameterMode::MUT
                            )
                            {
                                RuntimeValue* reference =
                                    resolveLValue(
                                        argumentNode
                                    );

                                if (reference == nullptr)
                                {
                                    throw std::runtime_error(
                                        "Noki function '" +
                                        functionName +
                                        "' parameter '" +
                                        parameter.getName() +
                                        "' requires a mutable lvalue."
                                    );
                                }

                                argumentReferences[i] =
                                    reference;
                            }
                            else
                            {
                                EvalResult argument =
                                    eval(argumentNode);

                                if (
                                    argument.control !=
                                    FlowControl::NONE
                                )
                                {
                                    throw std::runtime_error(
                                        "Invalid control flow in "
                                        "function argument."
                                    );
                                }

                                argumentValues[i] =
                                    std::move(
                                        argument.value
                                    );
                            }
                        }

                        /*
                         * Validar os tipos dos parâmetros.
                         */
                        for (size_t i = 0;
                             i < parameters.size();
                             ++i)
                        {
                            ASTNode::TypeNode expectedType =
                                parameters[i].getType();

                            bool valid = false;

                            RuntimeValue* value = nullptr;

                            if (
                                parameters[i].getMode() ==
                                FunctionParameterMode::MUT
                            )
                            {
                                value =
                                    argumentReferences[i];
                            }
                            else
                            {
                                value =
                                    &argumentValues[i];
                            }

                            switch (expectedType)
                            {
                                case TypeNode::ANYVALUE:
                                    valid = true;
                                    break;

                                case TypeNode::INT:
                                    valid =
                                        value->is<int64_t>();
                                    break;

                                case TypeNode::FLOAT:
                                    valid =
                                        value->is<double>();
                                    break;

                                case TypeNode::BOOL:
                                    valid =
                                        value->is<bool>();
                                    break;

                                case TypeNode::STRING:
                                    valid =
                                        value->is<std::string>();
                                    break;

                                case TypeNode::VECTOR:
                                    valid =
                                        value->isVector();
                                    break;

                                default:
                                    valid = false;
                                    break;
                            }

                            if (!valid)
                            {
                                std::string expectedTypeName;

                                switch (expectedType)
                                {
                                    case TypeNode::ANYVALUE:
                                        expectedTypeName = "any";
                                        break;

                                    case TypeNode::INT:
                                        expectedTypeName = "int";
                                        break;

                                    case TypeNode::FLOAT:
                                        expectedTypeName = "float";
                                        break;

                                    case TypeNode::BOOL:
                                        expectedTypeName = "bool";
                                        break;

                                    case TypeNode::STRING:
                                        expectedTypeName = "string";
                                        break;

                                    case TypeNode::VECTOR:
                                        expectedTypeName = "vector";
                                        break;

                                    default:
                                        expectedTypeName = "unknown";
                                        break;
                                }

                                throw std::runtime_error(
                                    "Noki function '" +
                                    functionName +
                                    "' parameter '" +
                                    parameters[i].getName() +
                                    "' expected type " +
                                    expectedTypeName +
                                    "."
                                );
                            }
                        }

                        /*
                         * Guard para trocar temporariamente para o
                         * environment da library.
                         */
                        struct EnvironmentSwitchGuard
                        {
                            Runtime& runtime;
                            Environment* previous;

                            EnvironmentSwitchGuard(
                                Runtime& runtime,
                                Environment* environment
                            )
                                : runtime(runtime),
                                  previous(runtime.current)
                            {
                                runtime.current =
                                    environment;
                            }

                            ~EnvironmentSwitchGuard()
                            {
                                runtime.current =
                                    previous;
                            }
                        };

                        /*
                         * Guard para criar e remover o environment
                         * da chamada da função.
                         */
                        struct EnvironmentPushGuard
                        {
                            Runtime& runtime;

                            explicit EnvironmentPushGuard(
                                Runtime& runtime
                            )
                                : runtime(runtime)
                            {
                                runtime.pushEnvironment();
                            }

                            ~EnvironmentPushGuard()
                            {
                                runtime.popEnvironment();
                            }
                        };

                        /*
                         * O environment do módulo passa a ser o
                         * environment atual.
                         */
                        EnvironmentSwitchGuard moduleGuard(
                            runtime,
                            moduleEnvironment
                        );

                        /*
                         * Criar o environment da chamada.
                         *
                         * O parent será o Environment do módulo.
                         */
                        EnvironmentPushGuard functionGuard(
                            runtime
                        );

                        /*
                         * Fazer o binding dos parâmetros.
                         */
                        for (size_t i = 0;
                             i < parameters.size();
                             ++i)
                        {
                            const FunctionParameter& parameter =
                                parameters[i];

                            if (
                                parameter.getMode() ==
                                FunctionParameterMode::MUT
                            )
                            {
                                runtime.current->createReference(
                                    parameter.getName(),
                                    argumentReferences[i]
                                );
                            }
                            else if (
                                parameter.getMode() ==
                                FunctionParameterMode::CONST
                            )
                            {
                                runtime.current->createConst(
                                    parameter.getName(),
                                    std::move(
                                        argumentValues[i]
                                    )
                                );
                            }
                            else
                            {
                                runtime.current->createVar(
                                    parameter.getName(),
                                    std::move(
                                        argumentValues[i]
                                    )
                                );
                            }
                        }

                        /*
                         * Executar o body da função.
                         */
                        EvalResult result =
                            eval(
                                nokiFunction->getBody()
                            );

                        /*
                         * RETURN pertence à função e é consumido aqui.
                         */
                        if (
                            result.control ==
                            FlowControl::RETURN
                        )
                        {
                            return {
                                std::move(result.value),
                                FlowControl::NONE
                            };
                        }

                        /*
                         * BREAK/CONTINUE não podem escapar
                         * de uma função.
                         */
                        if (
                            result.control ==
                                FlowControl::BREAK ||
                            result.control ==
                                FlowControl::CONTINUE
                        )
                        {
                            throw std::runtime_error(
                                "Invalid break or continue outside "
                                "of a while loop."
                            );
                        }

                        /*
                         * Função terminou sem return explícito.
                         */
                        return {
                            RuntimeValue(nullptr),
                            FlowControl::NONE
                        };
                    }
                }
            }
        }

        /*
         * Resolver o receiver.
         */
        RuntimeValue* receiver =
            resolveLValue(objectNode);

        if (receiver != nullptr)
        {
            /*
             * RuntimeObject:
             * usar exclusivamente o sistema de objetos existente.
             */
            if (receiver->isObject())
            {
                RuntimeObject* object =
                    receiver->getObject();

                if (object == nullptr)
                {
                    return {
                        RuntimeValue(nullptr),
                        FlowControl::NONE
                    };
                }

                RuntimeObjectType* type =
                    runtime.objects.findType(
                        object->getTypeID()
                    );

                if (type == nullptr)
                {
                    return {
                        RuntimeValue(nullptr),
                        FlowControl::NONE
                    };
                }

                const BuiltinEntry* method =
                    type->findMethod(
                        functionName
                    );

                if (method != nullptr)
                {
                    RuntimeValue result =
                        std::visit(
                            [&](const auto& func)
                                -> RuntimeValue
                            {
                                using T =
                                    std::decay_t<
                                        decltype(func)
                                    >;

                                if constexpr (
                                    std::is_same_v<
                                        T,
                                        BuiltinFunc
                                    >
                                )
                                {
                                    std::vector<
                                        RuntimeValue
                                    > args;

                                    args.reserve(
                                        node.children.size()
                                    );

                                    args.push_back(
                                        *receiver
                                    );

                                    for (
                                        size_t i = 1;
                                        i < node.children.size();
                                        ++i
                                    )
                                    {
                                        EvalResult argument =
                                            eval(
                                                *node.children[i]
                                            );

                                        if (
                                            argument.control !=
                                            FlowControl::NONE
                                        )
                                        {
                                            throw std::runtime_error(
                                                "Invalid control flow "
                                                "in function argument."
                                            );
                                        }

                                        args.push_back(
                                            std::move(
                                                argument.value
                                            )
                                        );
                                    }

                                    return func(args);
                                }
                                else if constexpr (
                                    std::is_same_v<
                                        T,
                                        BuiltinMutFunc
                                    >
                                )
                                {
                                    std::vector<
                                        RuntimeValue
                                    > temporaries;

                                    std::vector<
                                        RuntimeValue*
                                    > args;

                                    temporaries.reserve(
                                        node.children.size() - 1
                                    );

                                    args.reserve(
                                        node.children.size()
                                    );

                                    args.push_back(
                                        receiver
                                    );

                                    for (
                                        size_t i = 1;
                                        i < node.children.size();
                                        ++i
                                    )
                                    {
                                        RuntimeValue* reference =
                                            resolveLValue(
                                                *node.children[i]
                                            );

                                        if (
                                            reference != nullptr
                                        )
                                        {
                                            args.push_back(
                                                reference
                                            );
                                        }
                                        else
                                        {
                                            EvalResult argument =
                                                eval(
                                                    *node.children[i]
                                                );

                                            if (
                                                argument.control !=
                                                FlowControl::NONE
                                            )
                                            {
                                                throw std::runtime_error(
                                                    "Invalid control flow "
                                                    "in function argument."
                                                );
                                            }

                                            temporaries.push_back(
                                                std::move(
                                                    argument.value
                                                )
                                            );

                                            args.push_back(
                                                &temporaries.back()
                                            );
                                        }
                                    }

                                    return func(args);
                                }
                            },
                            method->function
                        );

                    return {
                        std::move(result),
                        FlowControl::NONE
                    };
                }
            }
            /*
             * RuntimeValue intrínseco:
             * resolver pelo RuntimeValueTypeID.
             */
            else
            {
                const RuntimeValueTypeID typeID =
                    receiver->getTypeID();

                RuntimeValueType* type =
                    runtime.objects.findValueType(
                        typeID
                    );

                if (type == nullptr)
                {
                    return {
                        RuntimeValue(nullptr),
                        FlowControl::NONE
                    };
                }

                const BuiltinEntry* method =
                    type->findMethod(
                        functionName
                    );

                if (method != nullptr)
                {
                    RuntimeValue result =
                        std::visit(
                            [&](const auto& func)
                                -> RuntimeValue
                            {
                                using T =
                                    std::decay_t<
                                        decltype(func)
                                    >;

                                if constexpr (
                                    std::is_same_v<
                                        T,
                                        BuiltinFunc
                                    >
                                )
                                {
                                    std::vector<
                                        RuntimeValue
                                    > args;

                                    args.reserve(
                                        node.children.size()
                                    );

                                    args.push_back(
                                        *receiver
                                    );

                                    for (
                                        size_t i = 1;
                                        i < node.children.size();
                                        ++i
                                    )
                                    {
                                        EvalResult argument =
                                            eval(
                                                *node.children[i]
                                            );

                                        if (
                                            argument.control !=
                                            FlowControl::NONE
                                        )
                                        {
                                            throw std::runtime_error(
                                                "Invalid control flow "
                                                "in function argument."
                                            );
                                        }

                                        args.push_back(
                                            std::move(
                                                argument.value
                                            )
                                        );
                                    }

                                    return func(args);
                                }
                                else if constexpr (
                                    std::is_same_v<
                                        T,
                                        BuiltinMutFunc
                                    >
                                )
                                {
                                    std::vector<
                                        RuntimeValue
                                    > temporaries;

                                    std::vector<
                                        RuntimeValue*
                                    > args;

                                    temporaries.reserve(
                                        node.children.size() - 1
                                    );

                                    args.reserve(
                                        node.children.size()
                                    );

                                    args.push_back(
                                        receiver
                                    );

                                    for (
                                        size_t i = 1;
                                        i < node.children.size();
                                        ++i
                                    )
                                    {
                                        RuntimeValue* reference =
                                            resolveLValue(
                                                *node.children[i]
                                            );

                                        if (
                                            reference != nullptr
                                        )
                                        {
                                            args.push_back(
                                                reference
                                            );
                                        }
                                        else
                                        {
                                            EvalResult argument =
                                                eval(
                                                    *node.children[i]
                                                );

                                            if (
                                                argument.control !=
                                                FlowControl::NONE
                                            )
                                            {
                                                throw std::runtime_error(
                                                    "Invalid control flow "
                                                    "in function argument."
                                                );
                                            }

                                            temporaries.push_back(
                                                std::move(
                                                    argument.value
                                                )
                                            );

                                            args.push_back(
                                                &temporaries.back()
                                            );
                                        }
                                    }

                                    return func(args);
                                }
                            },
                            method->function
                        );

                    return {
                        std::move(result),
                        FlowControl::NONE
                    };
                }
            }
        }

        /*
         * Não era RuntimeObject nem RuntimeValue:
         * continuar com o mecanismo atual de biblioteca C++.
         */
        const ASTNode& libraryNode =
            objectNode;

        if (libraryNode.getType() != TypeNode::VARIABLE)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::NONE
            };
        }

        const std::string libraryName(
            libraryNode.getString()
        );

        Library* library =
            runtime.libraries.findLibrary(
                libraryName
            );

        if (library == nullptr)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::NONE
            };
        }

        if (
            !runtime.libraries.isImported(
                libraryName
            )
        )
        {
            throw std::runtime_error(
                "Library '" +
                libraryName +
                "' is not imported."
            );
        }

        auto it =
            library->functions.find(
                functionName
            );

        if (it == library->functions.end())
        {
            throw std::runtime_error(
                "Function '" +
                functionName +
                "' not found in library '" +
                libraryName +
                "'."
            );
        }

        RuntimeValue result =
            std::visit(
                [&](const auto& func) -> RuntimeValue
                {
                    using T =
                        std::decay_t<
                            decltype(func)
                        >;

                    if constexpr (
                        std::is_same_v<
                            T,
                            BuiltinFunc
                        >
                    )
                    {
                        std::vector<RuntimeValue> args;

                        args.reserve(
                            node.children.size() - 1
                        );

                        for (
                            size_t i = 1;
                            i < node.children.size();
                            ++i
                        )
                        {
                            EvalResult argument =
                                eval(
                                    *node.children[i]
                                );

                            if (
                                argument.control !=
                                FlowControl::NONE
                            )
                            {
                                throw std::runtime_error(
                                    "Invalid control flow "
                                    "in function argument."
                                );
                            }

                            args.push_back(
                                std::move(
                                    argument.value
                                )
                            );
                        }

                        return func(args);
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            BuiltinMutFunc
                        >
                    )
                    {
                        std::vector<
                            RuntimeValue
                        > temporaries;

                        std::vector<
                            RuntimeValue*
                        > args;

                        temporaries.reserve(
                            node.children.size() - 1
                        );

                        args.reserve(
                            node.children.size() - 1
                        );

                        for (
                            size_t i = 1;
                            i < node.children.size();
                            ++i
                        )
                        {
                            RuntimeValue* reference =
                                resolveLValue(
                                    *node.children[i]
                                );

                            if (reference != nullptr)
                            {
                                args.push_back(
                                    reference
                                );
                            }
                            else
                            {
                                EvalResult argument =
                                    eval(
                                        *node.children[i]
                                    );

                                if (
                                    argument.control !=
                                    FlowControl::NONE
                                )
                                {
                                    throw std::runtime_error(
                                        "Invalid control flow "
                                        "in function argument."
                                    );
                                }

                                temporaries.push_back(
                                    std::move(
                                        argument.value
                                    )
                                );

                                args.push_back(
                                    &temporaries.back()
                                );
                            }
                        }

                        return func(args);
                    }
                },
                it->second.function
            );

        return {
            std::move(result),
            FlowControl::NONE
        };
    }

    throw std::runtime_error(
        "Invalid calling of a function '" +
        std::string(target.getString()) +
        "'."
    );
}

EvalResult Interpreter::evalUni(const ASTNode& node) {

    EvalResult resultvalue = eval(*node.children[0]);
    auto value = std::move(resultvalue.value);

    switch (node.getOp()) {
        case ASTNode::NEGATIVE:
        {
            if (value.is<bool>())
                return {RuntimeValue(!value.get<bool>()),FlowControl::NONE};
            else if (value.is<int64_t>())
                return {RuntimeValue(-(value.get<int64_t>())),FlowControl::NONE};
            else if (value.is<double>())
                return {RuntimeValue(-(value.get<double>())),FlowControl::NONE};
            else
                return {RuntimeValue(nullptr),FlowControl::NONE};
        }
    }
    return {RuntimeValue(nullptr),FlowControl::NONE};

}

EvalResult Interpreter::evalMove(
    const ASTNode& node
)
{
    if (node.children.size() != 2)
    {
        throw std::runtime_error(
            "Invalid move expression."
        );
    }

    const ASTNode& target =
        *node.children[0];

    const ASTNode& source =
        *node.children[1];

    if (target.getType() != TypeNode::VARIABLE)
    {
        throw std::runtime_error(
            "Move destination must be a variable."
        );
    }

    const std::string targetName(
        target.getString()
    );

    /*
     * Primeiro obtemos o novo valor.
     *
     * Se o RHS for uma variável, fazemos o move
     * diretamente do RuntimeValue armazenado nessa
     * variável, sem passar por evalVar() e portanto
     * sem fazer clone().
     */
    RuntimeValue value;

    if (source.getType() == TypeNode::VARIABLE)
    {
        const std::string sourceName(
            source.getString()
        );

        if (targetName == sourceName)
        {
            throw std::runtime_error(
                "Cannot move a variable into itself: " +
                sourceName
            );
        }

        RuntimeValue* sourceValue =
            runtime.current->findVar(sourceName);

        if (sourceValue == nullptr)
        {
            throw std::runtime_error(
                "Unknown variable: " +
                sourceName
            );
        }

        value =
            std::move(*sourceValue);

        if (!runtime.current->removeVar(sourceName))
        {
            throw std::runtime_error(
                "Failed to remove moved variable: " +
                sourceName
            );
        }
    }
    else
    {
        EvalResult result =
            eval(source);

        if (
            result.control !=
            FlowControl::NONE
        )
        {
            throw std::runtime_error(
                "Invalid control flow inside move expression."
            );
        }

        value =
            std::move(result.value);
    }

    /*
     * O destino perde completamente o valor anterior.
     */
    runtime.current->removeVar(
        targetName
    );

    /*
     * O novo valor passa para o destino por move.
     */
    runtime.current->createVar(
        targetName,
        std::move(value)
    );

    return {
        runtime.current->getVar(targetName),
        FlowControl::NONE
    };
}

EvalResult Interpreter::evalBlock(const ASTNode& node)
{
    runtime.pushEnvironment();

    EvalResult result{
        RuntimeValue(nullptr),
        FlowControl::NONE
    };

    for (const auto& child : node.children)
    {
        result = eval(*child);
        if (result.control != FlowControl::NONE)
        {
            runtime.popEnvironment();
            return result;
        }
    }

    runtime.popEnvironment();

    return result;
}

EvalResult Interpreter::evalIf(const ASTNode& node)
{
    EvalResult cond = eval(*node.children[0]); //def-left

    /*
    caso nao seja bool caira sempre para o null pq todas as comparaçoes dao bool  
    */
    if (cond.control != FlowControl::NONE)
    {
        throw std::runtime_error(
            "Invalid control flow in if condition."
        );
    }

    bool result = false; //deixar inicializado por padrao

    if (cond.value.is<bool>()) {
        result = cond.value.get<bool>();
    } else {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }
     
    if (result) return eval(*node.children[1]);

    //se tiver else:
    if (node.children.size() == 3)
        return eval(*node.children[2]);

    return {
        RuntimeValue(nullptr),
        FlowControl::NONE
    }; //failsafe
}

EvalResult Interpreter::evalWhile(const ASTNode& node)
{
    EvalResult result{
        RuntimeValue(nullptr),
        FlowControl::NONE
    };

    EvalResult condition =
        eval(*node.children[0]);

    if (condition.control != FlowControl::NONE)
    {
        throw std::runtime_error(
            "Invalid control flow in while condition."
        );
    }

    while (
        RuntimeValue::runtimeValueToBool(
            condition.value
        )
    )
    {
        result = eval(*node.children[1]);

        if (result.control == FlowControl::BREAK)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::NONE
            };
        }

        if (result.control == FlowControl::CONTINUE)
        {
            condition =
                eval(*node.children[0]);

            if (condition.control != FlowControl::NONE)
            {
                throw std::runtime_error(
                    "Invalid control flow in while condition."
                );
            }

            continue;
        }

        if (result.control == FlowControl::RETURN)
        {
            return result;
        }

        condition =
            eval(*node.children[0]);

        if (condition.control != FlowControl::NONE)
        {
            throw std::runtime_error(
                "Invalid control flow in while condition."
            );
        }
    }

    return {
        std::move(result.value),
        FlowControl::NONE
    };
}

EvalResult Interpreter::evalImport(const ASTNode& node)
{
    const std::string name(node.getString());

    if (runtime.libraries.importLibrary(name))
    {
        return {
            RuntimeValue(true),
            FlowControl::NONE
        };
    }

    if (runtime.libraries.importNokiLibrary(name))
    {
        return {
            RuntimeValue(true),
            FlowControl::NONE
        };
    }

    throw std::runtime_error(
        "Library not found: " + name
    );
}

EvalResult Interpreter::evalMemberAccess(const ASTNode& node)
{
    if (node.children.size() != 2)
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };

    const ASTNode& object = *node.children[0];
    const ASTNode& member = *node.children[1];

    if (object.getType() != TypeNode::VARIABLE)
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };

    if (member.getType() != TypeNode::VARIABLE)
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };

    std::string libraryName(object.getString());
    std::string memberName(member.getString());

    Library* library =
        runtime.libraries.findLibrary(libraryName);

    if (library == nullptr)
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };

    if (!runtime.libraries.isImported(libraryName))
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };

    auto it = library->functions.find(memberName);

    if (it == library->functions.end())
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };

    return {
        RuntimeValue(nullptr),
        FlowControl::NONE
    };
}

EvalResult Interpreter::evalVector(const ASTNode& node)
{
    auto vector = std::make_unique<RuntimeValue_Vector>();

    for (const auto& child : node.children)
    {
        EvalResult rvalue = eval(*child);

        if (rvalue.control != FlowControl::NONE)
        {
            throw std::runtime_error(
                "Invalid control flow inside vector expression."
            );
        }

        vector->pushback(
            std::move(rvalue.value)
        );
    }

    return {
        RuntimeValue(std::move(vector)),
        FlowControl::NONE
    };
}

EvalResult Interpreter::evalIndex(const ASTNode& node)
{
    if (node.children.size() != 2)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    EvalResult objectResult =
        eval(*node.children[0]);

    if (objectResult.control != FlowControl::NONE)
    {
        throw std::runtime_error(
            "Invalid control flow inside index expression."
        );
    }

    EvalResult indexResult =
        eval(*node.children[1]);

    if (indexResult.control != FlowControl::NONE)
    {
        throw std::runtime_error(
            "Invalid control flow inside index expression."
        );
    }

    RuntimeValue object = std::move(objectResult.value);
    RuntimeValue index = std::move(indexResult.value);

    if (!object.isVector())
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    if (!index.is<int64_t>())
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    auto* vector = object.getVector();

    if (vector == nullptr)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    int64_t indexValue = index.get<int64_t>();

    if (indexValue < 0)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    if (
        static_cast<size_t>(indexValue) >= vector->size()
    )
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    return {
        vector->at(static_cast<size_t>(indexValue)),
        FlowControl::NONE
    };
}

EvalResult Interpreter::evalFuncDecl(const ASTNode& node)
{
    if (node.getType() != TypeNode::FUNCTION_DECLARATION)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    if (node.children.size() != 2)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    const ASTNode& nameNode = *node.children[0];
    const ASTNode& bodyNode = *node.children[1];

    if (nameNode.getType() != TypeNode::VARIABLE)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    if (bodyNode.getType() != TypeNode::BLOCK)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    std::string name(nameNode.getString());

    if (runtime.current->existLocalNokiFunction(name))
    {
        throw std::runtime_error(
            "Noki function '" +
            name +
            "' is already declared in this scope."
        );
    }

    const auto& parameters =
        std::get<
            std::unique_ptr<std::vector<FunctionParameter>>
        >(node.as);

    if (!parameters)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    std::vector<FunctionParameter> functionParameters =
        *parameters;

    auto body = ASTNode::ast_clone(bodyNode);

    auto function = std::make_unique<NokiFunction>(
        std::move(name),
        std::move(functionParameters),
        std::move(body)
    );

    const std::string& functionName =
        function->getName();

    runtime.current->createNokiFunction(
        functionName,
        std::move(function)
    );

    return {
        RuntimeValue(nullptr),
        FlowControl::NONE
    };
}