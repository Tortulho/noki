#ifndef NOKI_DUMMY
#define NOKI_DUMMY

#include <memory>
#include "lib.hpp"

class RuntimeObjectManager;
class LibraryInitializer;

namespace dummy
{

    std::unique_ptr<LibraryInitializer> createInitializer(
        RuntimeObjectManager& manager
    );

    RuntimeValue hello(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue create(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue id(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue add(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue set(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue div(
        std::vector<RuntimeValue*>& args
    );
}

extern Library dummyLibrary;

#endif