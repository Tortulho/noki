#include "runtime/runtimeval.hpp"
#include <vector>
#include "builtin.hpp"
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

RuntimeValue builtin::Vecpushback(
    std::vector<RuntimeValue*>& args)
{
    if (args.size() < 2) return RuntimeValue(nullptr);

    RuntimeValue* target = args[0];

    if (target == nullptr) return RuntimeValue(nullptr);

    if (!target->isVector())
        throw std::runtime_error("pushback expects a vector.");

    RuntimeValue_Vector* vector = target->getVector();

    if (vector == nullptr)
        return RuntimeValue(nullptr);

    for (size_t i = 1; i < args.size(); ++i)
    {
        if (args[i] == nullptr)
            return RuntimeValue(nullptr);

        vector->pushback(*args[i]);
    }

    return RuntimeValue(nullptr);
}

RuntimeValue builtin::Vecinsert(
    std::vector<RuntimeValue*>& args)
{
    if (args.size() < 2) return RuntimeValue(nullptr);

    RuntimeValue* target = args[0];

    if (target == nullptr) return RuntimeValue(nullptr);

    if (!target->isVector())
        throw std::runtime_error("insert expects a vector.");

    RuntimeValue_Vector* vector = target->getVector();

    if (vector == nullptr) return RuntimeValue(nullptr);

#define indexarg args[1]

    if (!indexarg->is<int64_t>())
        throw std::runtime_error("insert expects a integer index.");

    int64_t index = indexarg->get<int64_t>();
    if (index < 0)
        index = static_cast<int64_t>(vector->size()) + index + 1;

    for (size_t i = 2; i < args.size(); ++i)
    {
        if (args[i] == nullptr)
            return RuntimeValue(nullptr);

        vector->insert(index,*args[i]);
        index++;
    }

    return RuntimeValue(nullptr);
}

RuntimeValue builtin::Vecpop(
    std::vector<RuntimeValue*>& args)
{
    if (args.size() != 2) return RuntimeValue(nullptr);

    RuntimeValue* target = args[0];

    if (target == nullptr) return RuntimeValue(nullptr);

    if (!target->isVector())
        throw std::runtime_error("pop expects a vector.");

    RuntimeValue_Vector* vector = target->getVector();

    if (vector == nullptr) return RuntimeValue(nullptr);

#define indexarg args[1]

    if (!indexarg->is<int64_t>())
        throw std::runtime_error("pop expects a integer index.");

    int64_t index = indexarg->get<int64_t>();
    if (index < 0)
        index = static_cast<int64_t>(vector->size()) + index + 1;

    vector->pop(index);

    return RuntimeValue(nullptr);
}

RuntimeValue builtin::Vecremove(
    std::vector<RuntimeValue*>& args)
{
    if (args.size() != 3)
        return RuntimeValue(nullptr);

    RuntimeValue* target = args[0];

    if (target == nullptr)
        return RuntimeValue(nullptr);

    if (!target->isVector())
        throw std::runtime_error(
            "remove expects a vector."
        );

    RuntimeValue_Vector* vector =
        target->getVector();

    if (vector == nullptr)
        return RuntimeValue(nullptr);

    if (args[2] == nullptr ||
        !args[2]->is<int64_t>())
    {
        throw std::runtime_error(
            "remove expects an integer count."
        );
    }

    int64_t count = args[2]->get<int64_t>();

    if (count < -1)
        throw std::runtime_error(
            "remove count cannot be negative."
        );

    if (args[1] == nullptr)
        return RuntimeValue(nullptr);

    vector->remove(
        static_cast<size_t>(count),
        *args[1]
    );

    return RuntimeValue(nullptr);
}

RuntimeValue builtin::Vecfind(
    const std::vector<RuntimeValue>& args)
{
    if (args.size() != 2)
        return RuntimeValue(nullptr);

    const RuntimeValue& target = args[0];

    if (!target.isVector())
        throw std::runtime_error(
            "find expects a vector."
        );

    const RuntimeValue_Vector* vector =
        target.getVector();

    if (vector == nullptr)
        return RuntimeValue(nullptr);

    return RuntimeValue(
        vector->find(args[1])
    );
}

const std::unordered_map<std::string,BuiltinEntry> builtin::funcs =
{
    {"say", {builtin::println}},
    {"println", {builtin::println}},
    {"print", {builtin::print}},
    {"getInput", {builtin::getInput}},
    {"int", {builtin::toInt}},
    {"float", {builtin::toFloat}},
    {"string", {builtin::toString}},
    {"pushback", {builtin::Vecpushback}},
    {"insert", {builtin::Vecinsert}},
    {"pop", {builtin::Vecpop}},
    {"find", {builtin::Vecfind}},
    {"remove", {builtin::Vecremove}},
};