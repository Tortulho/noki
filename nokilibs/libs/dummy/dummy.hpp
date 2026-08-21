#ifndef NOKI_DUMMY
#define NOKI_DUMMY

#include "../../lib.hpp"

namespace dummy
{
    RuntimeValue hello(const std::vector<RuntimeValue>& args);
    RuntimeValue create(const std::vector<RuntimeValue>& args);
    RuntimeValue id(std::vector<RuntimeValue*>& args);
}

const Library dummyLibrary
{
    "dummy",
    {
        {"hello", BuiltinEntry{dummy::hello}},
        {"create", BuiltinEntry{dummy::create}},
        {"id", BuiltinEntry{dummy::id}}
    }
};

extern const Library dummyLibrary;

#endif