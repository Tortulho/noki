#include "runtime/runtimeval.hpp"
#include <vector>
#include "builtin.hpp"
#include <string>
#include <iostream>

#define argc args.size()

RuntimeValue builtin::printlnVars(const std::vector<RuntimeValue>& args) {

    for (size_t idx = 0; idx < args.size(); idx++) {
        // if (args[idx].is<int64_t>()) {
        //     std::cout << (int64_t)args[idx].get<int64_t>();
        // } else if (args[idx].is<double>()) {
        //     std::cout << (double)args[idx].get<double>();
        // } else if (args[idx].is<std::string>()) {
        //     std::cout << (std::string)args[idx].get<std::string>();
        // } else if (args[idx].is<bool>()) {
        //     if (args[idx].get<bool>() == true)
        //     std::cout << "true";
        // }
        args[idx].print();
    }
    std::cout << '\n';
    return RuntimeValue(nullptr);
}

BuiltinFunc builtin::findFunction(const std::string& name)
{
    auto it = funcs.find(name);

    if (it == funcs.end()) return nullptr;

    return it->second;
}

// RuntimeValue builtin::sqrt(const std::vector<RuntimeValue>& args) {

//     if (argc != 1) return nullptr;

//     double val;

//     if (args[0].is<int64_t>()) {
//         int64_t num = args[0].get<int64_t>();
//         num = 
//     }

// }

const std::unordered_map<std::string, BuiltinFunc> builtin::funcs =
{
    {"say", &builtin::printlnVars},
    {"printlnVars", &builtin::printlnVars},
    //{"sqrt", &builtin::sqrt},
};