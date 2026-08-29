#include <memory>
#include <vector>
#include <string>

#include "lib.hpp"
#include "runtimeval.hpp"
#include "vectors.hpp"
#include "runtimeobjectManager.hpp"
#include "runtimevalType.hpp"
#include "libinitializer.hpp"

class VectorLibraryInitializer final
    : public LibraryInitializer
{
public:

    explicit VectorLibraryInitializer(
        RuntimeObjectManager& manager
    )
    {
        RuntimeValueType* type =
            manager.findValueType(
                RuntimeValueTypeID::VECTOR
            );

        if (type == nullptr)
        {
            throw std::runtime_error(
                "Failed to initialize Vector value type."
            );
        }

        type->registerMethod(
            "size",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    vector::size
                )
            }
        );

        type->registerMethod(
            "pushback",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    vector::pushback
                )
            }
        );

        type->registerMethod(
            "insert",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    vector::insert
                )
            }
        );

        type->registerMethod(
            "pop",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    vector::pop
                )
            }
        );

        type->registerMethod(
            "remove",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    vector::remove
                )
            }
        );

        type->registerMethod(
            "find",
            BuiltinEntry{
                static_cast<BuiltinFunc>(
                    vector::find
                )
            }
        );
    }
};

std::unique_ptr<LibraryInitializer>
vector::createInitializer(
    RuntimeObjectManager& manager
)
{
    return std::make_unique<VectorLibraryInitializer>(
        manager
    );
}

Library createVectorLibrary()
{
    return Library
    {
        "vector",
        {},
        {},
        vector::createInitializer
    };
}

namespace vector
{

    RuntimeValue pushback(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() < 2)
            return RuntimeValue(nullptr);

        RuntimeValue* target = args[0];

        if (target == nullptr)
            return RuntimeValue(nullptr);

        if (!target->isVector())
            throw std::runtime_error(
                "pushback expects a vector."
            );

        RuntimeValue_Vector* vector =
            target->getVector();

        if (vector == nullptr)
            return RuntimeValue(nullptr);

        for (size_t i = 1; i < args.size(); ++i)
        {
            if (args[i] == nullptr)
                return RuntimeValue(nullptr);

            vector->pushback(*args[i]);
        }

        return RuntimeValue(nullptr);
    }

    RuntimeValue insert(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() < 2)
            return RuntimeValue(nullptr);

        RuntimeValue* target = args[0];

        if (target == nullptr)
            return RuntimeValue(nullptr);

        if (!target->isVector())
            throw std::runtime_error(
                "insert expects a vector."
            );

        RuntimeValue_Vector* vector =
            target->getVector();

        if (vector == nullptr)
            return RuntimeValue(nullptr);

        RuntimeValue* indexArgument =
            args[1];

        if (indexArgument == nullptr)
            return RuntimeValue(nullptr);

        if (!indexArgument->is<int64_t>())
            throw std::runtime_error(
                "insert expects a integer index."
            );

        int64_t index =
            indexArgument->get<int64_t>();

        if (index < 0)
        {
            index =
                static_cast<int64_t>(
                    vector->size()
                ) + index + 1;
        }

        for (size_t i = 2; i < args.size(); ++i)
        {
            if (args[i] == nullptr)
                return RuntimeValue(nullptr);

            vector->insert(
                index,
                *args[i]
            );

            index++;
        }

        return RuntimeValue(nullptr);
    }

    RuntimeValue pop(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
            return RuntimeValue(nullptr);

        RuntimeValue* target = args[0];

        if (target == nullptr)
            return RuntimeValue(nullptr);

        if (!target->isVector())
            throw std::runtime_error(
                "pop expects a vector."
            );

        RuntimeValue_Vector* vector =
            target->getVector();

        if (vector == nullptr)
            return RuntimeValue(nullptr);

        RuntimeValue* indexArgument =
            args[1];

        if (indexArgument == nullptr)
            return RuntimeValue(nullptr);

        if (!indexArgument->is<int64_t>())
            throw std::runtime_error(
                "pop expects a integer index."
            );

        int64_t index =
            indexArgument->get<int64_t>();

        if (index < 0)
        {
            index =
                static_cast<int64_t>(
                    vector->size()
                ) + index + 1;
        }

        vector->pop(index);

        return RuntimeValue(nullptr);
    }

    RuntimeValue remove(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 3)
            return RuntimeValue(nullptr);

        RuntimeValue* target = args[0];

        if (target == nullptr)
            return RuntimeValue(nullptr);

        if (!target->isVector())
            throw std::runtime_error(
                "remove expects a vector."
            );

        RuntimeValue_Vector* vector =
            target->getVector();

        if (vector == nullptr)
            return RuntimeValue(nullptr);

        if (
            args[2] == nullptr ||
            !args[2]->is<int64_t>()
        )
        {
            throw std::runtime_error(
                "remove expects an integer count."
            );
        }

        int64_t count =
            args[2]->get<int64_t>();

        if (count < -1)
            throw std::runtime_error(
                "remove count cannot be negative."
            );

        if (args[1] == nullptr)
            return RuntimeValue(nullptr);

        vector->remove(
            static_cast<size_t>(count),
            *args[1]
        );

        return RuntimeValue(nullptr);
    }

    RuntimeValue find(
        const std::vector<RuntimeValue>& args
    )
    {
        if (args.size() != 2)
            return RuntimeValue(nullptr);

        const RuntimeValue& target =
            args[0];

        if (!target.isVector())
            throw std::runtime_error(
                "find expects a vector."
            );

        const RuntimeValue_Vector* vector =
            target.getVector();

        if (vector == nullptr)
            return RuntimeValue(nullptr);

        return RuntimeValue(
            vector->find(args[1])
        );
    }

    RuntimeValue size(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
            throw std::runtime_error(
                "Syntax error: vector.size() expects 0 arguments."
            );

        RuntimeValue* target = args[0];

        if (target == nullptr)
            throw std::runtime_error(
                "Runtime error: vector.size() received a null receiver."
            );

        if (!target->isVector())
            throw std::runtime_error(
                "Runtime error: vector.size() receiver must be a vector."
            );

        RuntimeValue_Vector* vector =
            target->getVector();

        if (vector == nullptr)
            return RuntimeValue(
                static_cast<int64_t>(0)
            );

        return RuntimeValue(
            static_cast<int64_t>(
                vector->size()
            )
        );
    }

}