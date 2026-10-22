#ifndef RUNNER_HPP
#define RUNNER_HPP

#include <string>
#include "interpreter.hpp"

class Runner
{
public:
    static EvalResult run(const std::string& source);
};

#endif