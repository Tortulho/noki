#include "dummy.hpp"
#include "../../../runtime/runtime.hpp"
#include "../../../runtime/objects/runtimeobject.hpp"

class DummyObject : public RuntimeObject
{
public:
    DummyObject(
        RuntimeObjectManager* manager,
        RuntimeObjectTypeID typeID
    )
        : RuntimeObject(manager, typeID)
    {
    }

    std::unique_ptr<RuntimeObject> clone() const override
    {
        RuntimeObjectManager* manager =
            getObjectManager();

        if (manager == nullptr)
        {
            return std::make_unique<DummyObject>(
                nullptr,
                getTypeID()
            );
        }

        return manager->createObject<DummyObject>(
            getTypeID()
        );
    }

    
};

namespace dummy
{
    RuntimeValue create(
        const std::vector<RuntimeValue>& args
    )
    {
        static const RuntimeObjectTypeID typeID =
            runtime.objects.registerType("Dummy");

        auto object =
            runtime.objects.createObject<DummyObject>(
                typeID
            );

        return RuntimeValue::fromObject(
            std::move(object)
        );
    }

    RuntimeValue hello(const std::vector<RuntimeValue>& args)
    {
        std::cout << "Hello from dummy library!\n";

        return RuntimeValue(nullptr);
    }

    RuntimeValue id(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 1)
            return RuntimeValue(nullptr);

        RuntimeValue* value = args[0];

        if (value == nullptr)
            return RuntimeValue(nullptr);

        if (!value->isObject())
            return RuntimeValue(nullptr);

        return RuntimeValue(
            static_cast<int64_t>(
                value->getObject()->getInstanceID()
            )
        );
    }
}