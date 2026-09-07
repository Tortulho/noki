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

    // Static library functions
    RuntimeValue open(const std::vector<RuntimeValue>& args);

    RuntimeValue set(const std::vector<RuntimeValue>& args);

    RuntimeValue create(const std::vector<RuntimeValue>& args);

    RuntimeValue fexists(const std::vector<RuntimeValue>& args);

    RuntimeValue frename(const std::vector<RuntimeValue>& args);

    RuntimeValue fdelete(const std::vector<RuntimeValue>& args);

    // FileObject methods
    RuntimeValue open(std::vector<RuntimeValue*>& args);

    RuntimeValue set(std::vector<RuntimeValue*>& args);

    RuntimeValue close(std::vector<RuntimeValue*>& args);

    RuntimeValue load(std::vector<RuntimeValue*>& args);

    RuntimeValue save(std::vector<RuntimeValue*>& args);

    RuntimeValue read(std::vector<RuntimeValue*>& args);

    RuntimeValue write(std::vector<RuntimeValue*>& args);

    RuntimeValue search(std::vector<RuntimeValue*>& args);

    RuntimeValue size(std::vector<RuntimeValue*>& args);

    RuntimeValue frename(std::vector<RuntimeValue*>& args);
}

Library createFileLibrary();

#endif