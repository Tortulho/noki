#ifndef NOKI_FILE
#define NOKI_FILE

#include <memory>
#include <vector>

#include "lib.hpp"
#include "runtimeval.hpp"

class RuntimeObjectManager;
class LibraryInitializer;

namespace file
{
    std::unique_ptr<LibraryInitializer> createInitializer(
        RuntimeObjectManager& manager
    );

    RuntimeValue open(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue open(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue close(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue load(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue save(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue read(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue write(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue search(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue size(std::vector<RuntimeValue*>& args);

    RuntimeValue create(const std::vector<RuntimeValue>& args);

}

Library createFileLibrary();

#endif