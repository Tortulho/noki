#ifndef RUNTIME
#define RUNTIME

#include <unordered_map>
#include <string>
#include <limits>
#include "runtimeval.hpp"
#include "environment.hpp"
#include "libmanager.hpp"
#include "runtimeobjectManager.hpp"
#include "nokiClass.hpp"

class Runtime
{
public:

    RuntimeObjectManager objects;
    LibraryManager libraries;
    Environment global;
    Environment* current;

    Runtime()
    : libraries(objects),
      global(nullptr),
      current(&global)
    {
        nokiClassMetaTypeID =
            objects.registerType("::noki::class-definition::");

        initializeLibraries();
    }

    inline void pushEnvironment() {
        current = new Environment(current);
    }
    inline void popEnvironment() {
        if (current == &global){
            throw std::runtime_error("Cannot pop global environment.");
            return;
        }
        Environment* old = current;
        current = current->getParent();
        delete old;
    }

    RuntimeObjectTypeID getNokiClassMetaTypeID()
    {
        if (
            nokiClassMetaTypeID ==
            std::numeric_limits<RuntimeObjectTypeID>::max()
        )
        {
            nokiClassMetaTypeID =
                objects.registerType(
                    "$noki$class-definition"
                );
        }

        return nokiClassMetaTypeID;
    }

private:
    void initializeLibraries();
    //void initializeValueTypes();
    RuntimeObjectTypeID nokiClassMetaTypeID =
    std::numeric_limits<RuntimeObjectTypeID>::max();

};

extern Runtime runtime;

#endif