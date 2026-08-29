#include "runtime.hpp"

//#include "dummy.hpp"
#include "file.hpp"
#include "strings.hpp"
#include "vectors.hpp"

Runtime runtime;

void Runtime::initializeLibraries()
{
    // libraries.registerLibrary(
    //     createDummyLibrary()
    // );

    libraries.registerLibrary(
        createFileLibrary()
    );

    libraries.registerLibrary(
        createStringLibrary()
    );

    libraries.registerLibrary(
        createVectorLibrary()
    );
}