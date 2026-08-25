#include "environment.hpp"
#include <unordered_map>
#include <string>
#include "runtimeval.hpp"

RuntimeValue* Environment::findVar(const std::string& name)
{
    auto it = variables.find(name);

    if (it != variables.end()) return &it->second;

    if (parent) return parent->findVar(name);

    return nullptr;
}

bool Environment::existLocal(const std::string& name) const
{
    auto it = variables.find(name);

    if (it != variables.end()) return true;
    return false;
}

bool Environment::exist(const std::string& name) const
{
    auto it = variables.find(name);

    if (it != variables.end()) return true;

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

bool Environment::assignVar(const std::string& name, RuntimeValue value) {

    auto it = variables.find(name);

    if (it != variables.end()) {
        it->second = std::move(value);
        return true;
    }

    if (parent) return parent->assignVar(name, std::move(value));

    return false;
}

void Environment::createVar(const std::string& name, RuntimeValue value) {
    variables[name] = std::move(value);
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