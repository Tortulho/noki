#ifndef NOKI_VECTOR
#define NOKI_VECTOR

#include <memory>
#include <vector>

#include "lib.hpp"
#include "runtimeval.hpp"

class RuntimeObjectManager;
class LibraryInitializer;

namespace vector
{
    std::unique_ptr<LibraryInitializer> createInitializer(
        RuntimeObjectManager& manager
    );

    RuntimeValue pushback(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue insert(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue pop(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue remove(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue find(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue size(
        std::vector<RuntimeValue*>& args
    );
}

Library createVectorLibrary();

#endif