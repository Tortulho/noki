#ifndef NOKI_TIME
#define NOKI_TIME

#include <memory>
#include <vector>

#include "lib.hpp"
#include "runtimeval.hpp"

class RuntimeObjectManager;
class LibraryInitializer;

namespace nokitime
{
    std::unique_ptr<LibraryInitializer>
    createInitializer(
        RuntimeObjectManager& manager
    );

    RuntimeValue now(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue getrealtimeyear(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue getrealtimemonth(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue getrealtimeday(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue getrealtimemin(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue getrealtimesec(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue getyear(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue getmonth(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue getday(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue gethour(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue getmin(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue getsec(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue getms(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue getns(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue set(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue set(
        const std::vector<RuntimeValue>& args
    );

    RuntimeValue setin_ns(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue setin_ms(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue setin_sec(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue setin_min(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue setin_hour(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue setin_day(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue setin_month(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue addin_ns(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue addin_ms(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue addin_sec(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue addin_min(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue addin_hour(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue addin_day(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue addin_month(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue addin_year(
        std::vector<RuntimeValue*>& args
    );

    RuntimeValue formated(
        std::vector<RuntimeValue*>& args
    );
}

Library createTimeLibrary();

#endif