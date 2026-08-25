#include <stdexcept>
#include <string>
#include <unordered_map>
#include "lib.hpp"
#include "libmanager.hpp"
#include "runtimeobjectManager.hpp"

/*

import dummy
    ↓
library existe?
    ↓
já importada?
    ↓
initializerFactory()
    ↓
RuntimeObjectTypeInitializer
    ↓
constructor → registerType()
    ↓
success
    ↓
guardar unique_ptr no LibraryManager
    ↓
marcar imported

*/

LibraryManager::LibraryManager(
    RuntimeObjectManager& objectManager
)
    : objectManager(objectManager)
{}

void LibraryManager::registerLibrary(const Library& library)
{
    if (libraries.find(library.name) != libraries.end()) {
        throw std::runtime_error(
            "Critical error: library '" +
            library.name +
            "' is already registered."
        );
    }

    libraries.emplace(library.name, library);
}

Library* LibraryManager::findLibrary(const std::string& name)
{
    auto it = libraries.find(name);

    if (it == libraries.end()) return nullptr;

    return &it->second;
}

const Library* LibraryManager::findLibrary(const std::string& name) const
{
    auto it = libraries.find(name);

    if (it == libraries.end()) return nullptr;

    return &it->second;
}

bool LibraryManager::exists(const std::string& name) const
{
    auto it = libraries.find(name);

    if (it == libraries.end()) return false;

    return true;
}

bool LibraryManager::importLibrary(
    const std::string& name
)
{
    if (!exists(name)) return false;
    if (isImported(name)) return true;

    Library* library = findLibrary(name);

    if (library == nullptr) return false;

    std::unique_ptr<LibraryInitializer> initializer;

    if (library->initializerFactory != nullptr)
    {
        initializer = library->initializerFactory(objectManager);

        if (initializer == nullptr) {
            throw std::runtime_error(
                "Library '" + name +
                "' failed to initialize: "
                "initializer factory returned null."
            );
        }
    }

    if (initializer != nullptr)
    {
        initializers.emplace(
            name,
            std::move(initializer)
        );
    }

    importedLibraries.insert(name);

    return true;
}

bool LibraryManager::isImported(const std::string& name) const
{
    auto it = importedLibraries.find(name);

    if (it == importedLibraries.end()) return false;

    return true;
}