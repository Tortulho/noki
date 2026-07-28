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

};

extern Runtime runtime;

#endif