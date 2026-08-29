#ifndef RUNTIMEOBJECTTYPE
#define RUNTIMEOBJECTTYPE

#include <cstdint>
#include <string>
#include <utility>
#include <unordered_map>

#include "builtinEntry.hpp"
#include "methodRegistry.hpp"

using RuntimeObjectTypeID = std::uint32_t;
using RuntimeObjectInstanceID = std::uint64_t;

class RuntimeObjectType
{
public:

    RuntimeObjectType(
        RuntimeObjectTypeID id,
        std::string name)
        : id(id),
          name(std::move(name))
    {
    }

    RuntimeObjectTypeID getID() const noexcept
    {
        return id;
    }

    const std::string& getName() const noexcept
    {
        return name;
    }

    void registerMethod(
        const std::string& name,
        BuiltinEntry entry
    );

    const BuiltinEntry* findMethod(
        const std::string& name
    ) const;

private:

    RuntimeObjectTypeID id;
    std::string name;

    MethodRegistry methods;
};

#endif