#ifndef RUNTIMEOBJECTMANAGER
#define RUNTIMEOBJECTMANAGER

#include <memory>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "runtimeobjectType.hpp"
#include "runtimeobject.hpp"
#include <type_traits>

class RuntimeObjectManager
{
public:

    RuntimeObjectTypeID registerType(const std::string& name);
    bool unregisterType(RuntimeObjectTypeID id);

    bool exists(RuntimeObjectTypeID id) const;
    bool exists(const std::string& name) const;

    RuntimeObjectType* findType(
        RuntimeObjectTypeID id);

    RuntimeObjectType* findType(
        const std::string& name);

    const RuntimeObjectType* findType(
        RuntimeObjectTypeID id) const;

    const RuntimeObjectType* findType(
        const std::string& name) const;

    RuntimeObjectInstanceID registerInstance(
        RuntimeObject& object);

    void unregisterInstance(
        RuntimeObjectInstanceID id);

    bool existsInstance(
        RuntimeObjectInstanceID id) const;

    RuntimeObject* findInstance(
        RuntimeObjectInstanceID id);

    const RuntimeObject* findInstance(
        RuntimeObjectInstanceID id) const;

    template<typename T, typename... Args>
    std::unique_ptr<T> createObject(
        RuntimeObjectTypeID typeID,
        Args&&... args);

private:

    // Type Registry
    RuntimeObjectTypeID nextID = 0;

    std::unordered_map<
        RuntimeObjectTypeID,
        RuntimeObjectType
    > types;

    std::unordered_map<
        std::string,
        RuntimeObjectTypeID
    > names;


    // Instance Registry
    RuntimeObjectInstanceID nextInstanceID = 0;

    std::unordered_map<
        RuntimeObjectInstanceID,
        RuntimeObject*
    > instances;
};

template<typename T, typename... Args>
    std::unique_ptr<T> RuntimeObjectManager::createObject(
        RuntimeObjectTypeID typeID,
        Args&&... args)
    {
        static_assert(
            std::is_base_of_v<RuntimeObject, T>,
            "T must derive from RuntimeObject"
        );

        auto object = std::make_unique<T>(
            this,
            typeID,
            std::forward<Args>(args)...
        );

        registerInstance(*object);

        return object;
    }

#endif