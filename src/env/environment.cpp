#include "environment.hpp"
#include <unordered_map>
#include <string>
#include "runtimeval.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

RuntimeValue* Environment::findVar(const std::string& name)
{
    auto ref = references.find(name);

    if (ref != references.end())
        return ref->second;

    auto constant = constValues.find(name);

    if (constant != constValues.end())
        return &constant->second;

    auto it = variables.find(name);

    if (it != variables.end())
        return &it->second;

    if (parent) return parent->findVar(name);

    return nullptr;
}

RuntimeValue* Environment::findLocalVar(const std::string& name)
{
    auto ref = references.find(name);

    if (ref != references.end())
        return ref->second;

    auto constant = constValues.find(name);

    if (constant != constValues.end())
        return &constant->second;

    auto it = variables.find(name);

    if (it != variables.end())
        return &it->second;

    return nullptr;
}

bool Environment::existLocal(const std::string& name) const
{
    if (references.find(name) != references.end())
        return true;

    if (constValues.find(name) != constValues.end())
        return true;

    if (variables.find(name) != variables.end())
        return true;

    return false;
}

bool Environment::exist(const std::string& name) const
{
    if (references.find(name) != references.end())
        return true;

    if (constValues.find(name) != constValues.end())
        return true;

    if (variables.find(name) != variables.end())
        return true;

    if (parent) return parent->exist(name);

    return false;
}

RuntimeValue& Environment::getVar(const std::string& name)
{
    RuntimeValue* value = findVar(name);

    if (value == nullptr) {
        throw std::runtime_error("Unknown variable: " + name);
    }

    return *value;
}

bool Environment::assignVar(
    const std::string& name,
    RuntimeValue value
)
{
    auto ref = references.find(name);

    if (ref != references.end())
    {
        *(ref->second) = std::move(value);
        return true;
    }

    auto constant = constValues.find(name);

    if (constant != constValues.end())
    {
        throw std::runtime_error(
            "Cannot assign to const value: " + name
        );
    }

    auto it = variables.find(name);

    if (it != variables.end())
    {
        it->second = std::move(value);
        return true;
    }

    if (parent)
        return parent->assignVar(
            name,
            std::move(value)
        );

    return false;
}

void Environment::createVar(
    const std::string& name,
    RuntimeValue value
)
{
    variables[name] = std::move(value);
}

void Environment::createConst(
    const std::string& name,
    RuntimeValue value
)
{
    constValues[name] = std::move(value);
}

void Environment::createReference(
    const std::string& name,
    RuntimeValue* value
)
{
    references[name] = value;
}

NokiFunction* Environment::findNokiFunction(const std::string& name)
{
    auto it = nokiFunctions.find(name);

    if (it != nokiFunctions.end())
        return it->second.get();

    if (parent)
        return parent->findNokiFunction(name);

    return nullptr;
}

const NokiFunction* Environment::findNokiFunction(const std::string& name) const
{
    auto it = nokiFunctions.find(name);

    if (it != nokiFunctions.end())
        return it->second.get();

    if (parent)
        return parent->findNokiFunction(name);

    return nullptr;
}

void Environment::createNokiFunction(
    const std::string& name,
    std::unique_ptr<NokiFunction> function
)
{
    nokiFunctions[name] = std::move(function);
}

bool Environment::removeVar(
    const std::string& name
)
{
    auto it = variables.find(name);

    if (it != variables.end())
    {
        variables.erase(it);
        return true;
    }

    if (parent)
        return parent->removeVar(name);

    return false;
}

Environment* Environment::getModuleEnvironment() noexcept
{
    Environment* module = this;

    while (module->parent != nullptr)
        module = module->parent;

    return module;
}

void Environment::registerNokiClassTypeID(
    RuntimeObjectTypeID id
)
{
    if (
        std::find(
            nokiClassTypeIDs.begin(),
            nokiClassTypeIDs.end(),
            id
        ) != nokiClassTypeIDs.end()
    )
    {
        throw std::runtime_error(
            "Noki class type ID already tracked."
        );
    }

    nokiClassTypeIDs.push_back(id);
}

void Environment::unregisterNokiClassTypeID(
    RuntimeObjectTypeID id
) noexcept
{
    auto it = std::find(
        nokiClassTypeIDs.begin(),
        nokiClassTypeIDs.end(),
        id
    );

    if (it != nokiClassTypeIDs.end())
        nokiClassTypeIDs.erase(it);
}

std::vector<RuntimeObjectTypeID>
Environment::takeNokiClassTypeIDs() noexcept
{
    return std::move(nokiClassTypeIDs);
}