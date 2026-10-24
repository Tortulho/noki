#ifndef NOKICLASSINSTANCE
#define NOKICLASSINSTANCE

#include <string>
#include <unordered_map>

#include "runtimeobject.hpp"
#include "runtimeval.hpp"

class Environment;

class NokiClassInstance final : public RuntimeObject
{
public:
    NokiClassInstance(
        RuntimeObjectManager* manager,
        RuntimeObjectTypeID typeID,
        RuntimeObjectInstanceID canonicalDefinitionID,
        Environment* moduleEnvironment
    );

    std::unique_ptr<RuntimeObject> clone() const override;

    RuntimeObjectInstanceID getCanonicalDefinitionID() const noexcept
    {
        return canonicalDefinitionID;
    }

    Environment* getModuleEnvironment() const noexcept
    {
        return moduleEnvironment;
    }

    RuntimeValue* findField(const std::string& name);

    const RuntimeValue* findField(
        const std::string& name
    ) const;

    void createField(
        const std::string& name,
        RuntimeValue value
    );

    std::unordered_map<std::string, RuntimeValue>& getFields() noexcept
    {
        return fields;
    }

    const std::unordered_map<std::string, RuntimeValue>&
    getFields() const noexcept
    {
        return fields;
    }

private:
    RuntimeObjectInstanceID canonicalDefinitionID;
    Environment* moduleEnvironment;

    std::unordered_map<std::string, RuntimeValue> fields;
};

#endif