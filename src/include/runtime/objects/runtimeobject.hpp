#ifndef RUNTIMEOBJECT
#define RUNTIMEOBJECT

#include <cstdint>
#include <limits>
#include <memory>

#include "runtimeobjectType.hpp"

class RuntimeObjectManager;

class RuntimeObject
{
public:

    virtual ~RuntimeObject();

    virtual std::unique_ptr<RuntimeObject> clone() const = 0;

    RuntimeObjectTypeID getTypeID() const noexcept;
    RuntimeObjectInstanceID getInstanceID() const noexcept;

protected:

    explicit RuntimeObject(RuntimeObjectTypeID typeID);

    RuntimeObject(
        RuntimeObjectManager* manager,
        RuntimeObjectTypeID typeID);

    RuntimeObjectManager* getObjectManager() const noexcept;

private:

    friend class RuntimeObjectManager;

    inline static constexpr RuntimeObjectInstanceID
        INVALID_INSTANCE_ID =
            std::numeric_limits<RuntimeObjectInstanceID>::max();

    RuntimeObjectTypeID typeID;
    RuntimeObjectInstanceID instanceID;
    RuntimeObjectManager* manager;
};

#endif