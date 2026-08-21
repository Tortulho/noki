#ifndef RUNTIMEOBJECTTYPE
#define RUNTIMEOBJECTTYPE

#include <cstdint>
#include <string>
#include <utility>

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

private:

    RuntimeObjectTypeID id;
    std::string name;
};

#endif