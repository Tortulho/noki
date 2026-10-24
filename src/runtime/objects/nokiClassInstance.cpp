#include "nokiClassInstance.hpp"
#include "nokiClass.hpp"
#include "runtimeobjectManager.hpp"
#include "environment.hpp"

#include <stdexcept>
#include <utility>

NokiClassInstance::NokiClassInstance(
    RuntimeObjectManager* manager,
    RuntimeObjectTypeID typeID,
    RuntimeObjectInstanceID canonicalDefinitionID,
    Environment* moduleEnvironment
)
    : RuntimeObject(manager, typeID),
      canonicalDefinitionID(canonicalDefinitionID),
      moduleEnvironment(moduleEnvironment)
{
    if (
        manager == nullptr ||
        moduleEnvironment == nullptr ||
        !manager->exists(typeID) ||
        canonicalDefinitionID == NokiClass::INVALID_CANONICAL_ID
    )
    {
        throw std::runtime_error(
            "Invalid Noki class instance."
        );
    }

    const RuntimeObject* object =
        manager->findInstance(canonicalDefinitionID);

    const auto* canonical =
        dynamic_cast<const NokiClass*>(object);

    if (
        canonical == nullptr ||
        !canonical->isCanonical()
    )
    {
        throw std::runtime_error(
            "Noki class instance has no valid class definition."
        );
    }
}

std::unique_ptr<RuntimeObject> NokiClassInstance::clone() const
{
    RuntimeObjectManager* manager = getObjectManager();

    if (manager == nullptr)
    {
        throw std::runtime_error(
            "Cannot clone an unregistered Noki class instance."
        );
    }

    const RuntimeObject* object =
        manager->findInstance(canonicalDefinitionID);

    const auto* canonical =
        dynamic_cast<const NokiClass*>(object);

    if (
        canonical == nullptr ||
        !canonical->isCanonical()
    )
    {
        throw std::runtime_error(
            "Cannot clone an instance without its class definition."
        );
    }

    auto copy =
        manager->createObject<NokiClassInstance>(
            getTypeID(),
            canonicalDefinitionID,
            moduleEnvironment
        );

    // Copiar os campos respeitando a semântica de cópia
    // de RuntimeValue, incluindo clone() para objetos.
    copy->fields = fields;

    return copy;
}

RuntimeValue* NokiClassInstance::findField(
    const std::string& name
)
{
    auto it = fields.find(name);

    if (it == fields.end())
        return nullptr;

    return &it->second;
}

const RuntimeValue* NokiClassInstance::findField(
    const std::string& name
) const
{
    auto it = fields.find(name);

    if (it == fields.end())
        return nullptr;

    return &it->second;
}

void NokiClassInstance::createField(
    const std::string& name,
    RuntimeValue value
)
{
    auto [it, inserted] =
        fields.emplace(name, std::move(value));

    if (!inserted)
    {
        throw std::runtime_error(
            "Duplicate field in Noki class instance: " + name
        );
    }
}