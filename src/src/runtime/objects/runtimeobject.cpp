#include "runtimeobject.hpp"
#include "runtimeobjectManager.hpp"
#include <stdexcept>

RuntimeObject::RuntimeObject(
    RuntimeObjectTypeID typeID)
    : typeID(typeID),
      instanceID(INVALID_INSTANCE_ID),
      manager(nullptr)
{
}

RuntimeObject::RuntimeObject(
    RuntimeObjectManager* manager,
    RuntimeObjectTypeID typeID)
    : typeID(typeID),
      instanceID(INVALID_INSTANCE_ID),
      manager(manager)
{
}

RuntimeObject::~RuntimeObject()
{
    if (manager != nullptr &&
        instanceID != INVALID_INSTANCE_ID)
    {
        manager->unregisterInstance(instanceID);
    }
}

RuntimeObjectTypeID RuntimeObject::getTypeID() const noexcept
{
    return typeID;
}

RuntimeObjectInstanceID RuntimeObject::getInstanceID() const noexcept
{
    return instanceID;
}

RuntimeObjectManager*
RuntimeObject::getObjectManager() const noexcept
{
    return manager;
}

//

