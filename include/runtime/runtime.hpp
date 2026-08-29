#ifndef RUNTIME
#define RUNTIME

#include <unordered_map>
#include <string>
#include "runtimeval.hpp"
#include "environment.hpp"
#include "libmanager.hpp"
#include "runtimeobjectManager.hpp"

class Runtime
{
public:

    RuntimeObjectManager objects;
    LibraryManager libraries;
    Environment global;
    Environment* current;

    Runtime()
        : libraries(objects),
        global(nullptr),
        current(&global)
    {
        initializeLibraries();
    }

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
    //void initializeValueTypes();

};

extern Runtime runtime;

#endif