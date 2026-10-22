#include "runtimeobjectType.hpp"

void RuntimeObjectType::registerMethod(
    const std::string& name,
    BuiltinEntry entry
)
{
    methods.registerMethod(
        name,
        std::move(entry)
    );
}

const BuiltinEntry* RuntimeObjectType::findMethod(
    const std::string& name
) const
{
    return methods.findMethod(name);
}
