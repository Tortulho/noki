#ifndef NOKI_STRING
#define NOKI_STRING

#include <memory>
#include <vector>

#include "lib.hpp"
#include "runtimeval.hpp"

class RuntimeObjectManager;
class LibraryInitializer;

namespace string
{
    std::unique_ptr<LibraryInitializer> createInitializer(
        RuntimeObjectManager& manager
    );

    RuntimeValue format(std::vector<RuntimeValue*>& args);
    RuntimeValue cat(std::vector<RuntimeValue*>& args);
    RuntimeValue find(std::vector<RuntimeValue*>& args);
    RuntimeValue replace(std::vector<RuntimeValue*>& args);
    RuntimeValue rfind(std::vector<RuntimeValue*>& args);
    RuntimeValue rreplace(std::vector<RuntimeValue*>& args);
    RuntimeValue remove(std::vector<RuntimeValue*>& args);
    RuntimeValue at(std::vector<RuntimeValue*>& args);
    RuntimeValue copy(std::vector<RuntimeValue*>& args);
    RuntimeValue strip(std::vector<RuntimeValue*>& args);
    RuntimeValue reserve(std::vector<RuntimeValue*>& args);
    RuntimeValue reverse(std::vector<RuntimeValue*>& args);
    RuntimeValue popback(std::vector<RuntimeValue*>& args);
    RuntimeValue size(std::vector<RuntimeValue*>& args);
    RuntimeValue isempty(std::vector<RuntimeValue*>& args);
    RuntimeValue empty(std::vector<RuntimeValue*>& args);
    RuntimeValue pop(std::vector<RuntimeValue*>& args);
}

Library createStringLibrary();

#endif