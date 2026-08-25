#include "runtime.hpp"
#include "dummy.hpp"

Runtime runtime;

// REGISTER LIBS HERE

void Runtime::initializeLibraries()
{
    libraries.registerLibrary(dummyLibrary);
}