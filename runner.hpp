#ifndef RUNNER_HPP
#define RUNNER_HPP

#include <string>
#include "runtime/runtimeval.hpp"

class Runner
{
public:
    static RuntimeValue run(const std::string& source);
};

#endif