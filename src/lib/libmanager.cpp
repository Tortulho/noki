#include <stdexcept>
#include <string>
#include <unordered_map>
#include "lib.hpp"
#include "libmanager.hpp"
#include "runtimeobjectManager.hpp"

#include <fstream>
#include <iterator>
#include <vector>

#include "ast.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "runtime.hpp"

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

void LibraryManager::registerLibrary(Library library)
{
    if (libraries.find(library.name) != libraries.end()) {
        throw std::runtime_error(
            "Critical error: library '" +
            library.name +
            "' is already registered."
        );
    }

    const std::string name = library.name;

    libraries.emplace(
        name,
        std::move(library)
    );
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

std::filesystem::path LibraryManager::resolveNokiLibrary(
    const std::string& name
) const
{
    std::filesystem::path path =
        std::filesystem::current_path() /
        "nokilibs" /
        "libnoki" /
        (name + ".noki");

    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error(
            "Noki library not found: " +
            path.string()
        );
    }

    if (!std::filesystem::is_regular_file(path))
    {
        throw std::runtime_error(
            "Noki library path is not a regular file: " +
            path.string()
        );
    }

    return std::filesystem::weakly_canonical(path);
}

bool LibraryManager::importNokiLibrary(
    const std::string& name
)
{
    if (isImported(name))
        return true;

    if (exists(name))
        return false;

    const std::filesystem::path path =
        resolveNokiLibrary(name);

    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open Noki library: " +
            path.string()
        );
    }

    std::string source{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    std::vector<Token> tokens;

    Token::tokenize(
        source,
        tokens
    );

    Parser parser(tokens);

    auto ast = parser.parse();

    if (ast == nullptr)
    {
        throw std::runtime_error(
            "Failed to parse Noki library: " +
            path.string()
        );
    }

    auto environment =
        std::make_unique<Environment>(nullptr);

    auto module =
        std::make_unique<NokiModule>(
            name,
            path,
            std::move(source),
            std::move(ast),
            std::move(environment)
        );

    NokiModule* modulePtr =
        module.get();

    Library library;

    library.name = name;

    library.nokiModules.emplace(
        name,
        std::move(module)
    );

    libraries.emplace(
        name,
        std::move(library)
    );

    Runtime* runtimeInstance = &runtime;

    Environment* previousEnvironment =
        runtimeInstance->current;

    runtimeInstance->current =
        modulePtr->getEnvironment();

    try
    {
        EvalResult result =
            Interpreter::eval(
                *modulePtr->getAST()
            );

        if (result.control != FlowControl::NONE)
        {
            runtimeInstance->current =
                previousEnvironment;

            throw std::runtime_error(
                "Invalid control flow escaped "
                "Noki library '" +
                name +
                "'."
            );
        }
    }
    catch (...)
    {
        runtimeInstance->current =
            previousEnvironment;

        libraries.erase(name);

        throw;
    }

    runtimeInstance->current =
        previousEnvironment;

    importedLibraries.insert(name);

    return true;
}