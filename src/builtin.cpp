#include "runtimeval.hpp"
#include <vector>
#include "builtin.hpp"
#include "builtinEntry.hpp"   
#include <string>
#include <iostream>
#include <charconv>

#define argc args.size()

RuntimeValue builtin::println(const std::vector<RuntimeValue>& args) {

    for (size_t idx = 0; idx < args.size(); idx++) {
        args[idx].print();
    }
    std::cout << '\n';
    return RuntimeValue(nullptr);
}

RuntimeValue builtin::print(const std::vector<RuntimeValue>& args) {

    for (size_t idx = 0; idx < args.size(); idx++) {
        args[idx].print();
    }
    return RuntimeValue(nullptr);
}

RuntimeValue builtin::toInt(const std::vector<RuntimeValue>& args) {

    if (argc != 1) return RuntimeValue(nullptr);
    
    if (args[0].is<int64_t>()) return RuntimeValue(args[0].get<int64_t>());

    if (args[0].is<double>()) {
        double val = args[0].get<double>();
        int64_t rtval;
        if (val > INT64_MAX) {
            rtval = INT64_MAX;
            fprintf(stderr,"Value to big to convert %.2lf, using %ld.\n",val,rtval);
        } else if (val < INT64_MIN) {
            rtval = INT64_MIN;
            fprintf(stderr,"Value to big to convert %.2lf, using %ld.\n",val,rtval);
        } else {
            rtval = (int64_t)val;
        }
        return RuntimeValue(int64_t(rtval));
    };

    if (args[0].is<bool>()) {
        bool val = args[0].get<bool>();
        int64_t rtval;
        if (val) rtval = 1;
        else rtval = 0;
        return RuntimeValue(int64_t(rtval));
    };

    if (args[0].is<std::string>()) {
        std::string_view str = args[0].get<std::string>();
        int64_t value;
        auto result = std::from_chars(
            str.data(),
            str.data() + str.size(),
            value
        );

        if (result.ec != std::errc{} || result.ptr != str.data() + str.size()) {
            return RuntimeValue(nullptr);
        }

        return RuntimeValue(value);
    };

    return RuntimeValue(nullptr);

}

RuntimeValue builtin::toFloat(const std::vector<RuntimeValue>& args) {

    if (argc != 1) return RuntimeValue(nullptr);
    
    if (args[0].is<double>()) return RuntimeValue(args[0].get<double>());

    if (args[0].is<int64_t>()) {
        double rtval = args[0].get<int64_t>();
        return RuntimeValue(double(rtval));
    };

    if (args[0].is<bool>()) {
        bool val = args[0].get<bool>();
        double rtval;
        if (val) rtval = 1.0;
        else rtval = 0.0;
        return RuntimeValue(double(rtval));
    };

    if (args[0].is<std::string>()) {
        std::string_view str = args[0].get<std::string>();
        double value;
        auto result = std::from_chars(
            str.data(),
            str.data() + str.size(),
            value
        );

        if (result.ec != std::errc{} || result.ptr != str.data() + str.size()) {
            return RuntimeValue(nullptr);
        }

        return RuntimeValue(value);
    };

    return RuntimeValue(nullptr);

}

RuntimeValue builtin::toString(const std::vector<RuntimeValue>& args) {

    if (argc != 1) return RuntimeValue(nullptr);

    const RuntimeValue& value = args[0];

    if (value.is<std::string>()) return value;

    if (value.is<int64_t>())
        return RuntimeValue(std::to_string(value.get<int64_t>()));

    if (value.is<double>())
        return RuntimeValue(std::to_string(value.get<double>()));

    if (value.is<bool>())
        return RuntimeValue(value.get<bool>() ? "true" : "false");

    return RuntimeValue(nullptr);

}


const BuiltinEntry* builtin::findFunction(const std::string& name)
{
    auto it = funcs.find(name);

    if (it == funcs.end()) return nullptr;

    return &it->second;
}

RuntimeValue builtin::getInput(const std::vector<RuntimeValue>& args)
{
    if (!args.empty()) return RuntimeValue(nullptr);
    std::string value;
    std::getline(std::cin, value);
    return RuntimeValue(value);
}

// RuntimeValue builtin::sqrt(const std::vector<RuntimeValue>& args) {

//     if (argc != 1) return nullptr;

//     double val;

//     if (args[0].is<int64_t>()) {
//         int64_t num = args[0].get<int64_t>();
//         num = 
//     }

// }


const std::unordered_map<std::string,BuiltinEntry> builtin::funcs =
{
    {"say", {builtin::println}},
    {"println", {builtin::println}},
    {"print", {builtin::print}},
    {"getInput", {builtin::getInput}},
    {"int", {builtin::toInt}},
    {"float", {builtin::toFloat}},
    {"string", {builtin::toString}},
};