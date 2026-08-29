#ifndef METHODREGISTRY
#define METHODREGISTRY

#include <string>
#include <unordered_map>

#include "builtinEntry.hpp"

class MethodRegistry
{
public:

    void registerMethod(
        const std::string& name,
        BuiltinEntry entry
    );

    const BuiltinEntry* findMethod(
        const std::string& name
    ) const;

private:

    std::unordered_map<
        std::string,
        BuiltinEntry
    > methods;
};

#endif