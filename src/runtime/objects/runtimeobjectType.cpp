#include "runtimeobjectType.hpp"

void RuntimeObjectType::registerMethod(
    const std::string& name,
    BuiltinEntry entry
)
{
    methods.emplace(
        name,
        std::move(entry)
    );
}

const BuiltinEntry* RuntimeObjectType::findMethod(
    const std::string& name
) const
{
    auto it = methods.find(name);

    if (it == methods.end()) return nullptr;

    return &it->second;
}

