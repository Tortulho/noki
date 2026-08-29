#ifndef RUNTIMEVALUETYPE
#define RUNTIMEVALUETYPE

#include "runtimevalTypeID.hpp"
#include "methodRegistry.hpp"

#include <utility>

class RuntimeValueType
{
public:

    explicit RuntimeValueType(
        RuntimeValueTypeID id
    )
        : id(id) {}

    RuntimeValueTypeID getID() const
    {
        return id;
    }

    void registerMethod(
        const std::string& name,
        BuiltinEntry entry
    )
    {
        methods.registerMethod(
            name,
            std::move(entry)
        );
    }

    const BuiltinEntry* findMethod(
        const std::string& name
    ) const
    {
        return methods.findMethod(name);
    }

private:

    RuntimeValueTypeID id;

    MethodRegistry methods;
};

#endif