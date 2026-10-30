#include "runtimeval.hpp"
#include "runtime.hpp"
#include "ast.hpp"
#include "asttypes.hpp"
#include "interpreter.hpp"
#include "builtin.hpp"
#include "builtinEntry.hpp"   
#include "runtimevalVector.hpp"
#include <math.h>

#include "nokiClass.hpp"
#include "nokiClassInstance.hpp"
#include "nokiModule.hpp"
#include "runtimevalVector.hpp"

#include <string>
#include <utility>

using TypeNode = ::TypeNode;

// Declarações antecipadas das funções auxiliares das classes Noki.

static bool canAccessNokiMember(
    const ASTNode& memberDeclaration,
    const NokiClassInstance& targetInstance
);

static bool canAccessNestedNokiClass(
    const NokiClass& parentClass,
    const NokiClass& nestedClass
);

static const NokiClass& getCanonicalNokiClass(
    const NokiClassInstance& instance
);

static const ASTNode* findNokiClassMethod(
    const ASTNode& declaration,
    const std::string& methodName
);

static const ASTNode* findNokiFieldDeclaration(
    const ASTNode& declaration,
    const std::string& fieldName
);

static const ASTNode* findNokiFieldDeclaration(
    const NokiClassInstance& instance,
    const std::string& fieldName
);

static bool nokiFieldTypeMatches(
    TypeNode fieldType,
    const RuntimeValue& value
);

static EvalResult instantiateNokiClass(
    const ASTNode& callNode,
    NokiClass& classDefinition
);

static bool canAccessNestedNokiClass(
    const NokiClass& parentClass,
    const NokiClass& nestedClass
)
{
    const ASTNode& declaration =
        nestedClass.getDeclaration();

    switch (declaration.accessModifier)
    {
    case ASTNode::AccessModifier::PUBLIC:
        return true;

    case ASTNode::AccessModifier::PRIVATE:
    case ASTNode::AccessModifier::PROTECTED:
        break;

    default:
        return false;
    }

    if (runtime.current == nullptr)
        return false;

    auto* activeInstance =
        dynamic_cast<NokiClassInstance*>(
            runtime.current->getExecutionReceiver()
        );

    if (activeInstance == nullptr)
        return false;

    return
        activeInstance->getCanonicalDefinitionID() ==
        parentClass.getCanonicalDefinitionID();
}

#define LEFTPARTOFTREE children[0]
#define RIGHTPARTOFTREE children[1]

RuntimeValue* Interpreter::resolveLValue(const ASTNode& node, FlowControl& control)
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

        RuntimeValue* object = resolveLValue(*node.children[0], control);

        if (object == nullptr || control == FlowControl::EXIT) return nullptr;

        EvalResult indexResult = eval(*node.children[1]);

        if (indexResult.control == FlowControl::EXIT)
        {
            control = FlowControl::EXIT;
            return nullptr;
        }

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

    if (node.getType() == TypeNode::MEMBER_ACCESS)
    {
        if (
            node.children.size() != 2 ||
            node.children[0] == nullptr ||
            node.children[1] == nullptr ||
            node.children[1]->getType() != TypeNode::VARIABLE
        )
        {
            return nullptr;
        }

        RuntimeValue* receiver =
            resolveLValue(*node.children[0], control);

        if (control == FlowControl::EXIT)
            return nullptr;

        if (
            receiver == nullptr ||
            !receiver->isObject()
        )
        {
            return nullptr;
        }

        auto* instance =
            dynamic_cast<NokiClassInstance*>(
                receiver->getObject()
            );

        if (instance == nullptr)
            return nullptr;

        const std::string fieldName(
            node.children[1]->getString()
        );

        const ASTNode* fieldDeclaration =
            findNokiFieldDeclaration(
                *instance,
                fieldName
            );

        if (fieldDeclaration == nullptr)
        {
            throw std::runtime_error(
                "Field '" + fieldName +
                "' not found in Noki class."
            );
        }

        if (!canAccessNokiMember(*fieldDeclaration, *instance))
        {
            throw std::runtime_error(
                "Access denied to field '" + fieldName + "'."
            );
        }

        RuntimeValue* field =
            instance->findField(fieldName);

        if (field == nullptr)
        {
            throw std::runtime_error(
                "Noki class field storage is inconsistent."
            );
        }

        return field;
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

        case TypeNode::CLASS_DECLARATION:
            return evalClassDeclaration(node);

        case TypeNode::EXIT:
            return {
                RuntimeValue(nullptr),
                FlowControl::EXIT
            };

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
        if (left.is<std::string>() && right.is<std::string>())
        {
            return {RuntimeValue(left.get<std::string>() + right.get<std::string>()),FlowControl::NONE};
        }
        if (left.is<std::string>() && right.is<std::int64_t>())
        {
            return {RuntimeValue(left.get<std::string>() + (std::to_string(right.get<std::int64_t>()))),FlowControl::NONE};
        }
        if (left.is<std::string>() && right.is<double>())
        {
            return {RuntimeValue(left.get<std::string>() + (std::to_string(right.get<double>()))),FlowControl::NONE};
        }
        if (left.is<std::string>() && right.is<bool>())
        {
            if (right.get<bool>() == true) {
                return {RuntimeValue(left.get<std::string>() + "true"),FlowControl::NONE};
            }
            return {RuntimeValue(left.get<std::string>() + "false"),FlowControl::NONE};
        }

        if (right.is<std::string>() && left.is<std::int64_t>())
        {
            return {RuntimeValue((std::to_string(left.get<std::int64_t>())) + right.get<std::string>()),FlowControl::NONE};
        }
        if (right.is<std::string>() && left.is<double>())
        {
            return {RuntimeValue((std::to_string(left.get<double>()))+ right.get<std::string>()),FlowControl::NONE};
        }
        if (right.is<std::string>() && left.is<bool>())
        {
            if (left.get<bool>() == true) {
                return {RuntimeValue("true" + right.get<std::string>()),FlowControl::NONE};
            }
            return {RuntimeValue("false" + right.get<std::string>()),FlowControl::NONE};
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

    if (resultvalue.control == FlowControl::EXIT)
        return resultvalue;

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

        return {value, FlowControl::NONE};
    }

    if (target.getType() == TypeNode::INDEX)
    {
        FlowControl control = FlowControl::NONE;

        RuntimeValue* targetValue =
            resolveLValue(target, control);

        if (control == FlowControl::EXIT)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::EXIT
            };
        }

        if (targetValue == nullptr)
            return {RuntimeValue(nullptr), FlowControl::NONE};

        *targetValue = std::move(value);

        return {*targetValue, FlowControl::NONE};
    }

    if (target.getType() == TypeNode::MEMBER_ACCESS)
    {
        FlowControl control = FlowControl::NONE;

        RuntimeValue* targetValue =
            resolveLValue(target, control);

        if (control == FlowControl::EXIT)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::EXIT
            };
        }

        if (targetValue == nullptr)
        {
            throw std::runtime_error(
                "Invalid Noki class field assignment."
            );
        }

        // Recuperar o campo declarado para validar o tipo.
        RuntimeValue* receiverValue =
            resolveLValue(*target.children[0], control);

        if (control == FlowControl::EXIT)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::EXIT
            };
        }

        if (
            receiverValue == nullptr ||
            !receiverValue->isObject()
        )
        {
            throw std::runtime_error(
                "Invalid Noki class field assignment."
            );
        }

        auto* instance =
            dynamic_cast<NokiClassInstance*>(
                receiverValue->getObject()
            );

        if (instance == nullptr)
        {
            throw std::runtime_error(
                "Assignment target is not a Noki class instance."
            );
        }

        const std::string fieldName(
            target.children[1]->getString()
        );

        const ASTNode* fieldDeclaration =
            findNokiFieldDeclaration(
                *instance,
                fieldName
            );

        if (fieldDeclaration == nullptr)
        {
            throw std::runtime_error(
                "Field '" + fieldName +
                "' not found in Noki class."
            );
        }

        if (
            !nokiFieldTypeMatches(
                fieldDeclaration->getTypeNode(),
                value
            )
        )
        {
            throw std::runtime_error(
                "Assignment type mismatch for Noki field '" +
                fieldName + "'."
            );
        }

        *targetValue = std::move(value);

        return {
            *targetValue,
            FlowControl::NONE
        };
    }

    return {RuntimeValue(nullptr), FlowControl::NONE};
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
        // Antes de procurar funções, verificar se o nome identifica
        // uma definição de classe guardada como RuntimeValue.
        RuntimeValue* namedValue =
            runtime.current->findVar(name);

        if (
            namedValue != nullptr &&
            namedValue->isObject()
        )
        {
            auto* classDefinition =
                dynamic_cast<NokiClass*>(
                    namedValue->getObject()
                );

            if (classDefinition != nullptr)
            {
                return instantiateNokiClass(
                    node,
                    *classDefinition
                );
            }
        }

        /*
         * Primeiro tentamos a resolução C++ existente.
         */
        const BuiltinEntry* entry = builtin::findFunction(name);

        if (entry != nullptr)
{
    FlowControl callControl = FlowControl::NONE;

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
                            argument.control ==
                            FlowControl::EXIT
                        )
                        {
                            callControl =
                                FlowControl::EXIT;

                            return RuntimeValue(nullptr);
                        }

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
                        FlowControl lvalueControl =
                            FlowControl::NONE;

                        RuntimeValue* reference =
                            resolveLValue(
                                *node.children[i],
                                lvalueControl
                            );

                        if (
                            lvalueControl ==
                            FlowControl::EXIT
                        )
                        {
                            callControl =
                                FlowControl::EXIT;

                            return RuntimeValue(nullptr);
                        }

                        if (reference != nullptr)
                        {
                            args.push_back(reference);
                        }
                        else
                        {
                            EvalResult argument =
                                eval(*node.children[i]);

                            if (
                                argument.control ==
                                FlowControl::EXIT
                            )
                            {
                                callControl =
                                    FlowControl::EXIT;

                                return RuntimeValue(nullptr);
                            }

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

                            temporaries.push_back(
                                std::move(argument.value)
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

    if (callControl == FlowControl::EXIT)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::EXIT
        };
    }

    return {
        std::move(result),
        FlowControl::NONE
    };
}
        /*
         * Depois tentamos uma função Noki local.
         */

         // Procurar primeiro um método da instância atual.
        if (
            auto* activeInstance =
                dynamic_cast<NokiClassInstance*>(
                    runtime.current->getExecutionReceiver()
                )
        )
        {
            const NokiClass& classDefinition =
                getCanonicalNokiClass(*activeInstance);

            const ASTNode* method =
                findNokiClassMethod(
                    classDefinition.getDeclaration(),
                    name
                );

            if (method != nullptr)
            {
                if (!canAccessNokiMember(*method, *activeInstance))
                {
                    throw std::runtime_error(
                        "Access denied to method '" + name + "'."
                    );
                }

                return evalNokiMethodCall(
                    node,
                    *method,
                    *activeInstance
                );
            }
        }
        NokiFunction* nokiFunction = runtime.current->findNokiFunction(name);

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
                    FlowControl control = FlowControl::NONE;

                    RuntimeValue* reference =
                        resolveLValue(argumentNode, control);

                    if (control == FlowControl::EXIT)
                    {
                        return {
                            RuntimeValue(nullptr),
                            FlowControl::EXIT
                        };
                    }

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

                    if (argument.control == FlowControl::EXIT)
                    {
                        return argument;
                    }

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

            // Uma função global não herda o receiver do chamador.
            runtime.current->setExecutionReceiver(nullptr);

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
            * EXIT propaga-se depois de remover o environment
            * desta chamada.
            */
            if (result.control == FlowControl::EXIT)
            {
                return result;
            }

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

                    if (runtime.libraries.isImported(libraryName))
                    {
                        RuntimeValue* moduleMember =
                            moduleEnvironment->findLocalVar(functionName);

                        if (
                            moduleMember != nullptr &&
                            moduleMember->isObject()
                        )
                        {
                            auto* classDefinition =
                                dynamic_cast<NokiClass*>(
                                    moduleMember->getObject()
                                );

                            if (classDefinition != nullptr)
                            {
                                return instantiateNokiClass(
                                    node,
                                    *classDefinition
                                );
                            }
                        }
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
                                FlowControl control =
                                    FlowControl::NONE;

                                RuntimeValue* reference =
                                    resolveLValue(
                                        argumentNode,
                                        control
                                    );

                                if (control == FlowControl::EXIT)
                                {
                                    return {
                                        RuntimeValue(nullptr),
                                        FlowControl::EXIT
                                    };
                                }

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
                                    argument.control ==
                                    FlowControl::EXIT
                                )
                                {
                                    return argument;
                                }

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

                        // Uma função de módulo não herda o receiver de uma instância.
                        runtime.current->setExecutionReceiver(nullptr);

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

                        if (result.control == FlowControl::EXIT)
                        {
                            return result;
                        }

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
        FlowControl receiverControl =
            FlowControl::NONE;

        RuntimeValue* receiver =
            resolveLValue(
                objectNode,
                receiverControl
            );

        if (receiverControl == FlowControl::EXIT)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::EXIT
            };
        }

        /*
        * Uma cadeia como Outer.Inner pode produzir um RuntimeValue
        * temporário que representa uma definição de classe.
        *
        * Mantemos o temporário vivo até terminar o despacho.
        */
        RuntimeValue evaluatedReceiver;

        if (
            receiver == nullptr &&
            objectNode.getType() == TypeNode::MEMBER_ACCESS
        )
        {
            EvalResult receiverResult = eval(objectNode);

            if (receiverResult.control == FlowControl::EXIT)
                return receiverResult;

            if (receiverResult.control != FlowControl::NONE)
            {
                throw std::runtime_error(
                    "Invalid control flow while resolving "
                    "a nested Noki class receiver."
                );
            }

            evaluatedReceiver = std::move(receiverResult.value);

            if (evaluatedReceiver.isObject())
            {
                receiver = &evaluatedReceiver;
            }
        }

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

                // Despacho de métodos de classes Noki.
                // Despacho de definições de classes aninhadas.
                if (auto* parentClass = dynamic_cast<NokiClass*>(object))
                {
                    RuntimeValue* nestedValue =
                        parentClass->findNestedClass(functionName);

                    if (nestedValue == nullptr)
                    {
                        throw std::runtime_error(
                            "Nested class '" + functionName +
                            "' not found in class '" +
                            parentClass->getClassName() + "'."
                        );
                    }

                    auto* nestedClass =
                        dynamic_cast<NokiClass*>(
                            nestedValue->getObject()
                        );

                    if (
                        nestedClass == nullptr ||
                        !nestedClass->isCanonical()
                    )
                    {
                        throw std::runtime_error(
                            "Invalid nested Noki class definition."
                        );
                    }

                    if (
                        !canAccessNestedNokiClass(
                            *parentClass,
                            *nestedClass
                        )
                    )
                    {
                        throw std::runtime_error(
                            "ACCESS DENIED: cannot instantiate nested class '" +
                            functionName + "'."
                        );
                    }

                    return instantiateNokiClass(
                        node,
                        *nestedClass
                    );
                }

                // Despacho de métodos de instâncias Noki.
                if (auto* instance = dynamic_cast<NokiClassInstance*>(object))
                {
                    const NokiClass& classDefinition =
                        getCanonicalNokiClass(*instance);

                    const ASTNode* method =
                        findNokiClassMethod(
                            classDefinition.getDeclaration(),
                            functionName
                        );

                    if (method == nullptr)
                    {
                        throw std::runtime_error(
                            "Method '" + functionName +
                            "' not found in Noki class '" +
                            classDefinition.getClassName() + "'."
                        );
                    }

                    if (!canAccessNokiMember(*method, *instance))
                    {
                        throw std::runtime_error(
                            "Access denied to method '" +
                            functionName +
                            "' in Noki class '" +
                            classDefinition.getClassName() +
                            "'."
                        );
                    }

                    return evalNokiMethodCall(
                        node,
                        *method,
                        *instance
                    );
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
                    FlowControl callControl =
                        FlowControl::NONE;

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
                                            argument.control ==
                                            FlowControl::EXIT
                                        )
                                        {
                                            callControl =
                                                FlowControl::EXIT;

                                            return RuntimeValue(nullptr);
                                        }

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
                                        FlowControl lvalueControl =
                                            FlowControl::NONE;

                                        RuntimeValue* reference =
                                            resolveLValue(
                                                *node.children[i],
                                                lvalueControl
                                            );

                                        if (
                                            lvalueControl ==
                                            FlowControl::EXIT
                                        )
                                        {
                                            callControl =
                                                FlowControl::EXIT;

                                            return RuntimeValue(nullptr);
                                        }

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
                                                argument.control ==
                                                FlowControl::EXIT
                                            )
                                            {
                                                callControl =
                                                    FlowControl::EXIT;

                                                return RuntimeValue(nullptr);
                                            }

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

                    if (callControl == FlowControl::EXIT)
                    {
                        return {
                            RuntimeValue(nullptr),
                            FlowControl::EXIT
                        };
                    }

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
                    FlowControl callControl =
                        FlowControl::NONE;

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
                                            argument.control ==
                                            FlowControl::EXIT
                                        )
                                        {
                                            callControl =
                                                FlowControl::EXIT;

                                            return RuntimeValue(nullptr);
                                        }

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
                                        FlowControl lvalueControl =
                                            FlowControl::NONE;

                                        RuntimeValue* reference =
                                            resolveLValue(
                                                *node.children[i],
                                                lvalueControl
                                            );

                                        if (
                                            lvalueControl ==
                                            FlowControl::EXIT
                                        )
                                        {
                                            callControl =
                                                FlowControl::EXIT;

                                            return RuntimeValue(nullptr);
                                        }

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
                                                argument.control ==
                                                FlowControl::EXIT
                                            )
                                            {
                                                callControl =
                                                    FlowControl::EXIT;

                                                return RuntimeValue(nullptr);
                                            }

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

                    if (callControl == FlowControl::EXIT)
                    {
                        return {
                            RuntimeValue(nullptr),
                            FlowControl::EXIT
                        };
                    }

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

        FlowControl callControl =
            FlowControl::NONE;

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
                                argument.control ==
                                FlowControl::EXIT
                            )
                            {
                                callControl =
                                    FlowControl::EXIT;

                                return RuntimeValue(nullptr);
                            }

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
                            FlowControl lvalueControl =
                                FlowControl::NONE;

                            RuntimeValue* reference =
                                resolveLValue(
                                    *node.children[i],
                                    lvalueControl
                                );

                            if (
                                lvalueControl ==
                                FlowControl::EXIT
                            )
                            {
                                callControl =
                                    FlowControl::EXIT;

                                return RuntimeValue(nullptr);
                            }

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
                                    argument.control ==
                                    FlowControl::EXIT
                                )
                                {
                                    callControl =
                                        FlowControl::EXIT;

                                    return RuntimeValue(nullptr);
                                }

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

        if (callControl == FlowControl::EXIT)
        {
            return {
                RuntimeValue(nullptr),
                FlowControl::EXIT
            };
        }

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

EvalResult Interpreter::evalMove(const ASTNode& node)
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
    
    if (cond.control == FlowControl::EXIT) return cond;

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

    EvalResult cond = eval(*node.children[0]);

    if (cond.control == FlowControl::EXIT) return cond;

    if (cond.control != FlowControl::NONE)
    {
        throw std::runtime_error(
            "Invalid control flow in while condition."
        );
    }

    while (
        RuntimeValue::runtimeValueToBool(
            cond.value
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
            cond = eval(*node.children[0]);

            if (cond.control == FlowControl::EXIT) return cond;

            if (cond.control != FlowControl::NONE)
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

        cond = eval(*node.children[0]);

        if (cond.control == FlowControl::EXIT) return cond;

        if (cond.control != FlowControl::NONE)
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

    // Primeiro, tentar importar uma biblioteca C++.
    if (runtime.libraries.importLibrary(name))
    {
        return {
            RuntimeValue(true),
            FlowControl::NONE
        };
    }

    // Depois, tentar importar uma biblioteca Noki.
    const NokiLibraryImportResult result =
        runtime.libraries.importNokiLibrary(name);

    switch (result)
    {
        case NokiLibraryImportResult::IMPORTED:
            return {
                RuntimeValue(true),
                FlowControl::NONE
            };

        case NokiLibraryImportResult::EXIT:
            return {
                RuntimeValue(nullptr),
                FlowControl::EXIT
            };

        case NokiLibraryImportResult::NOT_FOUND:
            throw std::runtime_error(
                "Library not found: " + name
            );
    }

    throw std::runtime_error(
        "Invalid Noki library import result."
    );
}

EvalResult Interpreter::evalMemberAccess(const ASTNode& node)
{
    if (
        node.children.size() != 2 ||
        node.children[0] == nullptr ||
        node.children[1] == nullptr
    )
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    const ASTNode& objectNode = *node.children[0];
    const ASTNode& memberNode = *node.children[1];

    if (memberNode.getType() != TypeNode::VARIABLE)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    const std::string memberName(
        memberNode.getString()
    );

    /*
     * 1. Resolver campos de instâncias.
     *
     * resolveLValue trata a identidade da instância,
     * a validação do campo e as regras de acesso.
     */
    FlowControl control = FlowControl::NONE;

    RuntimeValue* fieldValue =
        resolveLValue(node, control);

    if (control == FlowControl::EXIT)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::EXIT
        };
    }

    if (fieldValue != nullptr)
    {
        return {
            *fieldValue,
            FlowControl::NONE
        };
    }

    /*
     * 2. Resolver classes aninhadas.
     *
     * Exemplos:
     *     Outer.Inner
     *     Outer.Inner.Deep
     *     dummyA.Outer.Inner
     *
     * A expressão à esquerda pode ser uma variável que contém
     * NokiClass ou outra expressão MEMBER_ACCESS que resolve
     * para uma definição de classe.
     */
    RuntimeValue evaluatedBase;
    RuntimeValue* baseValue = nullptr;

    if (objectNode.getType() == TypeNode::VARIABLE)
    {
        baseValue =
            runtime.current->findVar(
                std::string(objectNode.getString())
            );
    }
    else if (
        objectNode.getType() == TypeNode::MEMBER_ACCESS
    )
    {
        EvalResult baseResult = eval(objectNode);

        if (baseResult.control == FlowControl::EXIT)
            return baseResult;

        if (baseResult.control != FlowControl::NONE)
        {
            throw std::runtime_error(
                "Invalid control flow while resolving "
                "a nested Noki class."
            );
        }

        evaluatedBase = std::move(baseResult.value);
        baseValue = &evaluatedBase;
    }

    if (
        baseValue != nullptr &&
        baseValue->isObject()
    )
    {
        auto* parentClass =
            dynamic_cast<NokiClass*>(
                baseValue->getObject()
            );

        if (parentClass != nullptr)
        {
            RuntimeValue* nestedValue =
                parentClass->findNestedClass(memberName);

            if (nestedValue == nullptr)
            {
                throw std::runtime_error(
                    "Nested class '" + memberName +
                    "' not found in class '" +
                    parentClass->getClassName() + "'."
                );
            }

            auto* nestedClass =
                dynamic_cast<NokiClass*>(
                    nestedValue->getObject()
                );

            if (
                nestedClass == nullptr ||
                !nestedClass->isCanonical()
            )
            {
                throw std::runtime_error(
                    "Invalid nested Noki class definition."
                );
            }

            if (
                !canAccessNestedNokiClass(
                    *parentClass,
                    *nestedClass
                )
            )
            {
                throw std::runtime_error(
                    "ACCESS DENIED: cannot access nested class '" +
                    memberName + "'."
                );
            }

            /*
             * A cópia do RuntimeValue cria uma referência
             * registada para a mesma definição canónica.
             * Não duplica a AST da classe.
             */
            return {
                *nestedValue,
                FlowControl::NONE
            };
        }
    }

    /*
     * 3. Resolver membros exportados por bibliotecas .noki.
     *
     * Este caminho só se aplica quando o receiver é o nome
     * simples de um módulo importado.
     */
    if (objectNode.getType() != TypeNode::VARIABLE)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    const std::string moduleName(
        objectNode.getString()
    );

    Library* library =
        runtime.libraries.findLibrary(moduleName);

    if (
        library == nullptr ||
        !runtime.libraries.isImported(moduleName)
    )
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    auto moduleIt =
        library->nokiModules.find(moduleName);

    if (
        moduleIt == library->nokiModules.end() ||
        moduleIt->second == nullptr
    )
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    Environment* moduleEnvironment =
        moduleIt->second->getEnvironment();

    if (moduleEnvironment == nullptr)
    {
        throw std::runtime_error(
            "Noki module has no environment: " +
            moduleName
        );
    }

    RuntimeValue* moduleValue =
        moduleEnvironment->findLocalVar(memberName);

    if (moduleValue == nullptr)
    {
        return {
            RuntimeValue(nullptr),
            FlowControl::NONE
        };
    }

    return {
        *moduleValue,
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

static const ASTNode* findNokiFieldDeclaration(
    const ASTNode& declaration,
    const std::string& fieldName
)
{
    for (const auto& member : declaration.children)
    {
        if (
            member == nullptr ||
            member->getType() !=
                TypeNode::CLASS_FIELD_DECLARATION ||
            member->children.size() != 2 ||
            member->children[0] == nullptr ||
            member->children[0]->getType() !=
                TypeNode::VARIABLE
        )
        {
            continue;
        }

        if (
            std::string(member->children[0]->getString()) ==
            fieldName
        )
        {
            return member.get();
        }
    }

    return nullptr;
}

static const NokiClass& getCanonicalClass(
    const NokiClassInstance& instance
)
{
    const RuntimeObject* object =
        runtime.objects.findInstance(
            instance.getCanonicalDefinitionID()
        );

    const auto* classDefinition =
        dynamic_cast<const NokiClass*>(object);

    if (
        classDefinition == nullptr ||
        !classDefinition->isCanonical()
    )
    {
        throw std::runtime_error(
            "Noki class instance has no valid canonical definition."
        );
    }

    return *classDefinition;
}

static const ASTNode* findNokiFieldDeclaration(
    const NokiClassInstance& instance,
    const std::string& fieldName
)
{
    const NokiClass& classDefinition =
        getCanonicalClass(instance);

    return findNokiFieldDeclaration(
        classDefinition.getDeclaration(),
        fieldName
    );
}

static RuntimeValue defaultNokiFieldValue(
    TypeNode fieldType
)
{
    switch (fieldType)
    {
    case TypeNode::INT:
    case TypeNode::ANYVALUE:
        return RuntimeValue(int64_t{0});

    case TypeNode::FLOAT:
        return RuntimeValue(0.0);

    case TypeNode::BOOL:
        return RuntimeValue(false);

    case TypeNode::STRING:
        return RuntimeValue(std::string{});

    case TypeNode::VECTOR:
        return RuntimeValue(
            std::make_unique<RuntimeValue_Vector>()
        );

    default:
        throw std::runtime_error(
            "Unsupported Noki class field type."
        );
    }
}

static bool nokiFieldTypeMatches(
    TypeNode fieldType,
    const RuntimeValue& value
)
{
    switch (fieldType)
    {
    case TypeNode::ANYVALUE:
        return true;

    case TypeNode::INT:
        return value.is<int64_t>();

    case TypeNode::FLOAT:
        return value.is<double>();

    case TypeNode::BOOL:
        return value.is<bool>();

    case TypeNode::STRING:
        return value.is<std::string>();

    case TypeNode::VECTOR:
        return value.isVector();

    default:
        return false;
    }
}

class NokiClassEnvironmentScope
{
public:
    explicit NokiClassEnvironmentScope(
        Environment* moduleEnvironment
    )
        : previous(runtime.current),
          module(moduleEnvironment)
    {
        if (module == nullptr)
        {
            throw std::runtime_error(
                "Noki class has no module environment."
            );
        }

        runtime.current = module;

        try
        {
            runtime.pushEnvironment();
        }
        catch (...)
        {
            runtime.current = previous;
            throw;
        }
    }

    ~NokiClassEnvironmentScope() noexcept
    {
        while (
            runtime.current != nullptr &&
            runtime.current != module &&
            runtime.current != &runtime.global
        )
        {
            try
            {
                runtime.popEnvironment();
            }
            catch (...)
            {
                break;
            }
        }

        runtime.current = previous;
    }

    NokiClassEnvironmentScope(
        const NokiClassEnvironmentScope&
    ) = delete;

    NokiClassEnvironmentScope& operator=(
        const NokiClassEnvironmentScope&
    ) = delete;

private:
    Environment* previous;
    Environment* module;
};

static EvalResult instantiateNokiClass(
    const ASTNode& callNode,
    NokiClass& classDefinition
)
{
    // Na fase 1, o construtor não recebe argumentos.
    if (callNode.children.size() != 1)
    {
        throw std::runtime_error(
            "Noki class '" +
            classDefinition.getClassName() +
            "' does not support constructor arguments yet."
        );
    }

    const ASTNode& declaration =
        classDefinition.getDeclaration();

    auto instance =
        runtime.objects.createObject<NokiClassInstance>(
            classDefinition.getClassTypeID(),
            classDefinition.getCanonicalDefinitionID(),
            classDefinition.getModuleEnvironment()
        );

    std::vector<const ASTNode*> fields;
    fields.reserve(declaration.children.size());

    // Criar primeiro todos os campos, com valores predefinidos.
    for (const auto& member : declaration.children)
    {
        if (
            member == nullptr ||
            member->getType() !=
                TypeNode::CLASS_FIELD_DECLARATION
        )
        {
            continue;
        }

        if (
            member->children.size() != 2 ||
            member->children[0] == nullptr ||
            member->children[0]->getType() !=
                TypeNode::VARIABLE ||
            member->children[1] == nullptr
        )
        {
            throw std::runtime_error(
                "Invalid Noki class field declaration."
            );
        }

        const std::string fieldName(
            member->children[0]->getString()
        );

        instance->createField(
            fieldName,
            defaultNokiFieldValue(member->getTypeNode())
        );

        fields.push_back(member.get());
    }

    // Inicializar os campos no contexto do módulo.
    {
        NokiClassEnvironmentScope scope(
            classDefinition.getModuleEnvironment()
        );
        runtime.current->setExecutionReceiver(instance.get());

        // Todas as referências são criadas apenas depois de
        // existirem todos os campos da instância.
        for (const ASTNode* field : fields)
        {
            const std::string fieldName(
                field->children[0]->getString()
            );

            RuntimeValue* value =
                instance->findField(fieldName);

            if (value == nullptr)
            {
                throw std::runtime_error(
                    "Noki class field storage is inconsistent."
                );
            }

            runtime.current->createReference(
                fieldName,
                value
            );
        }

        // Avaliar os inicializadores pela ordem da declaração.
        for (const ASTNode* field : fields)
        {
            const std::string fieldName(
                field->children[0]->getString()
            );

            EvalResult initialValue =
                Interpreter::eval(*field->children[1]);

            if (initialValue.control == FlowControl::EXIT)
            {
                return {
                    RuntimeValue(nullptr),
                    FlowControl::EXIT
                };
            }

            if (initialValue.control != FlowControl::NONE)
            {
                throw std::runtime_error(
                    "Invalid control flow in initializer for field '" +
                    fieldName + "'."
                );
            }

            const TypeNode fieldType =
                field->getTypeNode();

            if (
                !nokiFieldTypeMatches(
                    fieldType,
                    initialValue.value
                )
            )
            {
                throw std::runtime_error(
                    "Initializer type mismatch for Noki field '" +
                    fieldName + "'."
                );
            }

            RuntimeValue* fieldValue =
                instance->findField(fieldName);

            if (fieldValue == nullptr)
            {
                throw std::runtime_error(
                    "Noki class field storage is inconsistent."
                );
            }

            *fieldValue = std::move(initialValue.value);
        }
    }

    return {
        RuntimeValue::fromObject(std::move(instance)),
        FlowControl::NONE
    };
}

EvalResult Interpreter::evalClassDeclaration(
    const ASTNode& node
)
{
    if (
        node.getType() != TypeNode::CLASS_DECLARATION ||
        node.children.empty() ||
        node.children[0] == nullptr ||
        node.children[0]->getType() != TypeNode::VARIABLE
    )
    {
        throw std::runtime_error(
            "Invalid Noki class declaration AST."
        );
    }

    Environment* moduleEnvironment =
        runtime.current->getModuleEnvironment();

    // As declarações de topo pertencem ao ambiente do módulo.
    // As classes aninhadas são processadas recursivamente
    // dentro do registo da definição exterior.
    if (runtime.current != moduleEnvironment)
    {
        throw std::runtime_error(
            "Noki class declarations must be at module scope."
        );
    }

    const std::string className(
        node.children[0]->getString()
    );

    const std::string moduleName =
        moduleEnvironment->getModuleName();

    if (className.empty() || moduleName.empty())
    {
        throw std::runtime_error(
            "Noki class requires a valid name and module."
        );
    }

    if (
        moduleEnvironment->existLocal(className) ||
        moduleEnvironment->existLocalNokiFunction(className) ||
        builtin::findFunction(className) != nullptr
    )
    {
        throw std::runtime_error(
            "Noki class name conflicts with an existing symbol: " +
            className
        );
    }

    const std::string qualifiedName =
        moduleName + "::" + className;

    if (runtime.objects.exists(qualifiedName))
    {
        throw std::runtime_error(
            "Noki class type is already registered: " +
            qualifiedName
        );
    }

    // Clonar a AST antes de alterar os registos.
    auto declarationCopy =
        ASTNode::ast_clone(node);

    if (declarationCopy == nullptr)
    {
        throw std::runtime_error(
            "Failed to clone Noki class declaration."
        );
    }

    const RuntimeObjectTypeID metaTypeID =
        runtime.getNokiClassMetaTypeID();

    // Reservar o primeiro ID antes de o registar, de modo
    // que push_back não tenha de alocar depois do registo.
    std::vector<RuntimeObjectTypeID> registeredTypeIDs;
    registeredTypeIDs.reserve(1);

    const RuntimeObjectTypeID classTypeID =
        runtime.objects.registerType(qualifiedName);

    registeredTypeIDs.push_back(classTypeID);

    try
    {
        moduleEnvironment->registerNokiClassTypeID(
            classTypeID
        );

        auto classObject =
            runtime.objects.createObject<NokiClass>(
                metaTypeID,
                moduleName,
                className,
                qualifiedName,
                classTypeID,
                moduleEnvironment,
                std::move(declarationCopy)
            );

        classObject->establishCanonicalIdentity();

        // Registar todas as definições aninhadas antes
        // de publicar a classe no ambiente do módulo.
        classObject->registerNestedClasses(
            runtime.objects,
            metaTypeID,
            registeredTypeIDs
        );

        RuntimeValue classValue =
            RuntimeValue::fromObject(
                std::move(classObject)
            );

        moduleEnvironment->createConst(
            className,
            std::move(classValue)
        );
    }
    catch (...)
    {
        // Os objetos temporários da classe e dos seus filhos
        // são destruídos antes de entrar neste handler.
        bool rollbackFailed = false;

        for (
            auto it = registeredTypeIDs.rbegin();
            it != registeredTypeIDs.rend();
            ++it
        )
        {
            moduleEnvironment->unregisterNokiClassTypeID(
                *it
            );

            if (!runtime.objects.unregisterType(*it))
            {
                rollbackFailed = true;
            }
        }

        if (rollbackFailed)
        {
            throw std::runtime_error(
                "Critical error: failed to roll back "
                "Noki class type registrations."
            );
        }

        throw;
    }

    return {
        RuntimeValue(nullptr),
        FlowControl::NONE
    };
}

static const NokiClass& getCanonicalNokiClass(
    const NokiClassInstance& instance
)
{
    const RuntimeObject* object =
        runtime.objects.findInstance(
            instance.getCanonicalDefinitionID()
        );

    const auto* classDefinition =
        dynamic_cast<const NokiClass*>(object);

    if (
        classDefinition == nullptr ||
        !classDefinition->isCanonical()
    )
    {
        throw std::runtime_error(
            "Noki class instance has no valid canonical definition."
        );
    }

    return *classDefinition;
}

static const ASTNode* findNokiClassMethod(
    const ASTNode& declaration,
    const std::string& methodName
)
{
    const ASTNode* found = nullptr;

    for (const auto& member : declaration.children)
    {
        if (
            member == nullptr ||
            member->getType() !=
                TypeNode::FUNCTION_DECLARATION ||
            member->children.size() != 2 ||
            member->children[0] == nullptr ||
            member->children[0]->getType() !=
                TypeNode::VARIABLE
        )
        {
            continue;
        }

        if (
            member->children[0]->getString() != methodName
        )
        {
            continue;
        }

        if (found != nullptr)
        {
            throw std::runtime_error(
                "Duplicate method '" + methodName +
                "' in Noki class declaration."
            );
        }

        found = member.get();
    }

    return found;
}

EvalResult Interpreter::evalNokiMethodCall(
    const ASTNode& callNode,
    const ASTNode& methodDeclaration,
    NokiClassInstance& instance
)
{
    if (
        callNode.getType() != TypeNode::FUNCTION_CALL ||
        callNode.children.empty() ||
        methodDeclaration.getType() !=
            TypeNode::FUNCTION_DECLARATION ||
        methodDeclaration.children.size() != 2 ||
        methodDeclaration.children[1] == nullptr
    )
    {
        throw std::runtime_error(
            "Invalid Noki method call."
        );
    }

    const auto* parameterStorage =
        std::get_if<
            std::unique_ptr<std::vector<FunctionParameter>>
        >(&methodDeclaration.as);

    if (
        parameterStorage == nullptr ||
        !*parameterStorage
    )
    {
        throw std::runtime_error(
            "Invalid parameter list in Noki method."
        );
    }

    const auto& parameters = **parameterStorage;

    const size_t argumentCount =
        callNode.children.size() - 1;

    if (argumentCount != parameters.size())
    {
        throw std::runtime_error(
            "Noki method expected " +
            std::to_string(parameters.size()) +
            " arguments, got " +
            std::to_string(argumentCount) + "."
        );
    }

    // Avaliar os argumentos no contexto do chamador.
    std::vector<RuntimeValue> argumentValues(argumentCount);
    std::vector<RuntimeValue*> argumentReferences(
        argumentCount,
        nullptr
    );

    for (size_t i = 0; i < parameters.size(); ++i)
    {
        const FunctionParameter& parameter = parameters[i];
        const ASTNode& argumentNode =
            *callNode.children[i + 1];

        if (
            parameter.getMode() ==
            FunctionParameterMode::MUT
        )
        {
            FlowControl control = FlowControl::NONE;

            RuntimeValue* reference =
                resolveLValue(argumentNode, control);

            if (control == FlowControl::EXIT)
            {
                return {
                    RuntimeValue(nullptr),
                    FlowControl::EXIT
                };
            }

            if (reference == nullptr)
            {
                throw std::runtime_error(
                    "Noki method parameter '" +
                    parameter.getName() +
                    "' requires a mutable lvalue."
                );
            }

            argumentReferences[i] = reference;
        }
        else
        {
            EvalResult argument = eval(argumentNode);

            if (argument.control == FlowControl::EXIT)
                return argument;

            if (argument.control != FlowControl::NONE)
            {
                throw std::runtime_error(
                    "Invalid control flow in Noki method argument."
                );
            }

            argumentValues[i] = std::move(argument.value);
        }
    }

    // Validar os tipos dos argumentos.
    for (size_t i = 0; i < parameters.size(); ++i)
    {
        RuntimeValue* value =
            parameters[i].getMode() ==
                FunctionParameterMode::MUT
                ? argumentReferences[i]
                : &argumentValues[i];

        if (
            value == nullptr ||
            !nokiFieldTypeMatches(
                parameters[i].getType(),
                *value
            )
        )
        {
            throw std::runtime_error(
                "Type mismatch for Noki method parameter '" +
                parameters[i].getName() + "'."
            );
        }
    }

    const NokiClass& classDefinition =
        getCanonicalNokiClass(instance);

    // Entrar no âmbito do módulo que definiu a classe.
    NokiClassEnvironmentScope scope(
        instance.getModuleEnvironment()
    );

    // Este ambiente representa o contexto da instância.
    runtime.current->setExecutionReceiver(&instance);

    // Os campos são referências aos RuntimeValue da instância.
    for (auto& [fieldName, fieldValue] : instance.getFields())
    {
        runtime.current->createReference(
            fieldName,
            &fieldValue
        );
    }

    // Criar um ambiente separado para parâmetros e variáveis locais.
    runtime.pushEnvironment();

    for (size_t i = 0; i < parameters.size(); ++i)
    {
        const FunctionParameter& parameter = parameters[i];

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

    // Executar diretamente o corpo conservado na AST canónica.
    EvalResult result = eval(
        *methodDeclaration.children[1]
    );

    if (result.control == FlowControl::EXIT)
        return result;

    if (result.control == FlowControl::RETURN)
    {
        return {
            std::move(result.value),
            FlowControl::NONE
        };
    }

    if (
        result.control == FlowControl::BREAK ||
        result.control == FlowControl::CONTINUE
    )
    {
        throw std::runtime_error(
            "Invalid break or continue outside a while loop."
        );
    }

    if (result.control != FlowControl::NONE)
    {
        throw std::runtime_error(
            "Invalid control flow in Noki method."
        );
    }

    return {
        RuntimeValue(nullptr),
        FlowControl::NONE
    };
}

static bool canAccessNokiMember(
    const ASTNode& memberDeclaration,
    const NokiClassInstance& targetInstance
)
{
    if (
        memberDeclaration.accessModifier ==
        ASTNode::AccessModifier::PUBLIC
    )
    {
        return true;
    }

    // Nesta fase, private e protected só são acessíveis
    // no contexto de execução da mesma classe.
    if (
        memberDeclaration.accessModifier !=
            ASTNode::AccessModifier::PRIVATE &&
        memberDeclaration.accessModifier !=
            ASTNode::AccessModifier::PROTECTED
    )
    {
        return false;
    }

    if (runtime.current == nullptr)
        return false;

    RuntimeObject* receiver =
        runtime.current->getExecutionReceiver();

    auto* activeInstance =
        dynamic_cast<NokiClassInstance*>(receiver);

    if (activeInstance == nullptr)
        return false;

    return
        activeInstance->getCanonicalDefinitionID() ==
        targetInstance.getCanonicalDefinitionID();
}