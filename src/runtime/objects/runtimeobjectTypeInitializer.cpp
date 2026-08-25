#include "runtimeobjectTypeInitializer.hpp"
#include <exception>

RuntimeObjectTypeInitializer::RuntimeObjectTypeInitializer(
    RuntimeObjectManager& manager,
    const std::string& name
)
    : manager(manager),
      typeID(manager.registerType(name))
{
}

RuntimeObjectTypeInitializer::~RuntimeObjectTypeInitializer()
{
    if (!manager.unregisterType(typeID))
        std::terminate();
}

RuntimeObjectTypeID
RuntimeObjectTypeInitializer::getTypeID() const noexcept
{
    return typeID;
}

RuntimeObjectType*
RuntimeObjectTypeInitializer::getType() noexcept
{
    return manager.findType(typeID);
}