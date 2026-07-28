#include "runtime/runtimeval.hpp"
#include <vector>
#include <unordered_map>

using BuiltinFunc = RuntimeValue (*)(const std::vector<RuntimeValue>&);
struct BuiltinEntry {
    const char* name;
    BuiltinFunc function;
};

class builtin {

    public:
        static RuntimeValue printlnVars(const std::vector<RuntimeValue>& args);

    public:
        static const std::unordered_map<std::string, BuiltinFunc> funcs;

        static BuiltinFunc findFunction(const std::string& name);
        static RuntimeValue sqrt(const std::vector<RuntimeValue>& args);

};
