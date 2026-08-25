#ifndef NOKILIB_MAIN
#define NOKILIB_MAIN

#include <memory>
#include <string>
#include <unordered_map>
#include "builtin.hpp"
#include "builtinEntry.hpp"   
#include "libinitializer.hpp"

class RuntimeObjectManager;

using LibraryInitializerFactory =
    std::unique_ptr<LibraryInitializer> (*)(
        RuntimeObjectManager&
    );

class Library {
public:
    std::string name;
    std::unordered_map<std::string, BuiltinEntry> functions;

    LibraryInitializerFactory initializerFactory = nullptr;

};

#endif