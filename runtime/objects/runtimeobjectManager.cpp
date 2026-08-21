#include "runtimeobjectManager.hpp"
#include <stdexcept>

RuntimeObjectTypeID RuntimeObjectManager::registerType(
    const std::string& name)
{
    if (names.find(name) != names.end())
    {
        throw std::runtime_error(
            "Critical error: runtime object type '" +
            name +
            "' is already registered."
        );
    }

    RuntimeObjectTypeID id = nextID++;

    types.emplace(
        id,
        RuntimeObjectType(id, name)
    );

    names.emplace(name, id);

    return id;
}

bool RuntimeObjectManager::exists(
    RuntimeObjectTypeID id) const
{
    return types.find(id) != types.end();
}

bool RuntimeObjectManager::exists(
    const std::string& name) const
{
    return names.find(name) != names.end();
}

const RuntimeObjectType*
RuntimeObjectManager::findType(
    RuntimeObjectTypeID id) const
{
    auto it = types.find(id);

    if (it == types.end()) return nullptr;

    return &it->second;
}

const RuntimeObjectType*
RuntimeObjectManager::findType(
    const std::string& name) const
{
    auto it = names.find(name);

    if (it == names.end()) return nullptr;

    return findType(it->second);
}

RuntimeObjectInstanceID RuntimeObjectManager::registerInstance(
    RuntimeObject& object)
{
    if (object.instanceID != RuntimeObject::INVALID_INSTANCE_ID)
    {
        throw std::runtime_error(
            "Critical error: runtime object is already registered."
        );
    }

    RuntimeObjectInstanceID id = nextInstanceID++;

    auto [it, inserted] =
        instances.emplace(id, &object);

    if (!inserted)
    {
        throw std::runtime_error(
            "Critical error: runtime object instance ID collision."
        );
    }

    object.manager = this;
    object.instanceID = id;

    return id;
}

void RuntimeObjectManager::unregisterInstance(
    RuntimeObjectInstanceID id)
{
    auto it = instances.find(id);

    if (it == instances.end())
    {
        throw std::runtime_error(
            "Critical error: runtime object instance is not registered."
        );
    }

    RuntimeObject* object = it->second;

    if (object == nullptr)
    {
        throw std::runtime_error(
            "Critical error: runtime object registry contains null instance."
        );
    }

    if (object->manager != this ||
        object->instanceID != id)
    {
        throw std::runtime_error(
            "Critical error: runtime object registry is inconsistent."
        );
    }

    object->manager = nullptr;
    object->instanceID =
        RuntimeObject::INVALID_INSTANCE_ID;

    instances.erase(it);
}

bool RuntimeObjectManager::existsInstance(
    RuntimeObjectInstanceID id) const
{
    return instances.find(id) != instances.end();
}

RuntimeObject* RuntimeObjectManager::findInstance(
    RuntimeObjectInstanceID id)
{
    auto it = instances.find(id);

    if (it == instances.end())
        return nullptr;

    return it->second;
}

const RuntimeObject*
RuntimeObjectManager::findInstance(
    RuntimeObjectInstanceID id) const
{
    auto it = instances.find(id);

    if (it == instances.end())
        return nullptr;

    return it->second;
}