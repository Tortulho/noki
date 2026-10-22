#ifndef BUILTIN
#define BUILTIN

#include "runtimeval.hpp"
#include <vector>
#include <unordered_map>
#include "builtinEntry.hpp"    

// //this is a const func
// using BuiltinFunc = RuntimeValue (*)(const std::vector<RuntimeValue>&);
// //this is a func that can change things by inputing to them
// using BuiltinMutFunc = RuntimeValue (*)(std::vector<RuntimeValue*>&);
// // struct BuiltinEntry {
// //     const char* name;
// //     BuiltinFunc function;
// // };
// using BuiltinImplementation =
//     std::variant<
//         BuiltinFunc,
//         BuiltinMutFunc
//     >;

// struct BuiltinEntry {
//     BuiltinImplementation function;
// };

class builtin {

    public:
        static RuntimeValue println(const std::vector<RuntimeValue>& args);
        static RuntimeValue print(const std::vector<RuntimeValue>& args);
        static RuntimeValue getInput(const std::vector<RuntimeValue>& args);
        static RuntimeValue toInt(const std::vector<RuntimeValue>& args);
        static RuntimeValue toFloat(const std::vector<RuntimeValue>& args);
        static RuntimeValue toString(const std::vector<RuntimeValue>& args);

        static RuntimeValue type(const std::vector<RuntimeValue>& args);
        static RuntimeValue isInt(const std::vector<RuntimeValue>& args);
        static RuntimeValue isFloat(const std::vector<RuntimeValue>& args);
        static RuntimeValue isString(const std::vector<RuntimeValue>& args);
        static RuntimeValue isBool(const std::vector<RuntimeValue>& args);
        static RuntimeValue isNull(const std::vector<RuntimeValue>& args);

    public:
        //static const std::unordered_map<std::string, BuiltinFunc> funcs;
        static const std::unordered_map<std::string,BuiltinEntry> funcs;

        static const BuiltinEntry* findFunction(const std::string& name);
        //static RuntimeValue sqrt(const std::vector<RuntimeValue>& args);

};

#endif