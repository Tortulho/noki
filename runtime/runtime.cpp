#include "runtime.hpp"
#include "../nokilibs/libs/dummy/dummy.hpp"

Runtime runtime;

// REGISTER LIBS HERE

void Runtime::initializeLibraries()
{
    libraries.registerLibrary(dummyLibrary);
}