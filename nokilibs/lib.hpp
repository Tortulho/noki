#ifndef NOKILIB_MAIN
#define NOKILIB_MAIN

#include <string>
#include <unordered_map>
#include "../builtin.hpp"

class Library {
public:
    std::string name;
    std::unordered_map<std::string, BuiltinEntry> functions;
};

#endif