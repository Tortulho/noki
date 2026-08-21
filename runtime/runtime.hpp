#ifndef RUNTIME
#define RUNTIME

#include <unordered_map>
#include <string>
#include "runtimeval.hpp"
#include "../env/environment.hpp"
#include "../nokilibs/libmanager.hpp"
#include "objects/runtimeobjectManager.hpp"

class Runtime
{
public:

    RuntimeObjectManager objects;
    Environment global;
    Environment* current;
    LibraryManager libraries;

    inline void pushEnvironment() {
        current = new Environment(current);
    }
    inline void popEnvironment() {
        if (current == &global){
            throw std::runtime_error("Cannot pop global environment.");
            return;
        }
        Environment* old = current;
        current = current->getParent();
        delete old;
    }

private:
    void initializeLibraries();

public: 
    Runtime()
        : global(nullptr),
        current(&global) {
            initializeLibraries();
        }

};

extern Runtime runtime;

#endif