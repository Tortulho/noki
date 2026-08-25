#ifndef BUILTINENTRY
#define BUILTINENTRY

#include <vector>
#include <variant>

class RuntimeValue;

// função somente leitura
using BuiltinFunc =
    RuntimeValue (*)(const std::vector<RuntimeValue>&);

// função que pode modificar os argumentos
using BuiltinMutFunc =
    RuntimeValue (*)(std::vector<RuntimeValue*>&);

using BuiltinImplementation =
    std::variant<
        BuiltinFunc,
        BuiltinMutFunc
    >;

struct BuiltinEntry
{
    BuiltinImplementation function;
};

#endif