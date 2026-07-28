#ifndef RUNTIME
#define RUNTIME

#include <unordered_map>
#include <string>
#include "runtimeval.hpp"
#include "../env/environment.hpp"

class Runtime
{
public:

    Environment global;
    Environment* current;

    Runtime()
    : global(nullptr),
      current(&global) {}

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

};

extern Runtime runtime;

#endif