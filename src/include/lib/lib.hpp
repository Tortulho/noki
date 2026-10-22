#ifndef NOKILIB_MAIN
#define NOKILIB_MAIN

#include <memory>
#include <string>
#include <unordered_map>

#include "builtin.hpp"
#include "builtinEntry.hpp"
#include "libinitializer.hpp"
#include "nokiModule.hpp"

class RuntimeObjectManager;
//class NokiModule;

using LibraryInitializerFactory =
    std::unique_ptr<LibraryInitializer> (*)(
        RuntimeObjectManager&
    );

class Library
{
public:

    std::string name;

    std::unordered_map<
        std::string,
        BuiltinEntry
    > functions;

    std::unordered_map<
        std::string,
        std::unique_ptr<NokiModule>
    > nokiModules;

    LibraryInitializerFactory initializerFactory =
        nullptr;

};

#endif