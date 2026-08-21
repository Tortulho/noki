#ifndef NOKI_DUMMY
#define NOKI_DUMMY

#include "../../lib.hpp"

namespace dummy
{
    RuntimeValue hello(const std::vector<RuntimeValue>& args);
}

const Library dummyLibrary
{
    "dummy",
    {
        {"hello", dummy::hello}
    }
};

extern const Library dummyLibrary;

#endif