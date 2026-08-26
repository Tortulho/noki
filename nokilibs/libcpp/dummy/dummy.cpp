#include "dummy.hpp"
#include "runtime.hpp"
#include "runtimeobject.hpp"
#include "runtimeobjectTypeInitializer.hpp"
#include "libinitializer.hpp"

class DummyLibraryInitializer final
    : public LibraryInitializer,
      public RuntimeObjectTypeInitializer
{
public:
    explicit DummyLibraryInitializer(
        RuntimeObjectManager& manager
    )
        : RuntimeObjectTypeInitializer(manager, "Dummy")
    {
        RuntimeObjectType* type = getType();

        if (type == nullptr)
            throw std::runtime_error(
                "Failed to initialize Dummy type."
            );

        type->registerMethod(
            "id",
            BuiltinEntry{dummy::id}
        );
        type->registerMethod(
            "add",
            BuiltinEntry{dummy::add}
        );
        type->registerMethod(
            "set",
            BuiltinEntry{dummy::set}
        );
        type->registerMethod(
            "div",
            BuiltinEntry{dummy::div}
        );
    }
};

class DummyObject : public RuntimeObject
{
public:

    DummyObject(
        RuntimeObjectManager* manager,
        RuntimeObjectTypeID typeID
    )
        : RuntimeObject(manager, typeID),
        value(0)
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

    int64_t value;

};

namespace dummy
{

    std::unique_ptr<LibraryInitializer>
    createInitializer(RuntimeObjectManager& manager)
    {
        return std::make_unique<DummyLibraryInitializer>(manager);
    }

    RuntimeValue create(const std::vector<RuntimeValue>& args)
    {
        RuntimeObjectType* type = runtime.objects.findType("Dummy");

        if (type == nullptr)
            throw std::runtime_error(
                "Dummy type is not initialized."
            );

        auto object = runtime.objects.createObject<DummyObject>(type->getID());

        return RuntimeValue::fromObject(std::move(object));
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

    RuntimeValue add(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 2)
            return RuntimeValue(nullptr);

        RuntimeValue* receiver = args[0];
        RuntimeValue* value = args[1];

        if (receiver == nullptr || value == nullptr)
            return RuntimeValue(nullptr);

        if (!receiver->isObject())
            return RuntimeValue(nullptr);

        if (!value->is<int64_t>())
            return RuntimeValue(nullptr);

        DummyObject* object =
            dynamic_cast<DummyObject*>(receiver->getObject());

        if (object == nullptr)
            return RuntimeValue(nullptr);

        object->value += value->get<int64_t>();

        return RuntimeValue(object->value);
    }

    RuntimeValue set(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 2) return RuntimeValue(nullptr);

        RuntimeValue* receiver = args[0];
        RuntimeValue* value = args[1];

        if (receiver == nullptr || value == nullptr)
            return RuntimeValue(nullptr);

        if (!receiver->isObject())
            return RuntimeValue(nullptr);

        if (!value->is<int64_t>())
            return RuntimeValue(nullptr);

        DummyObject* object =
            dynamic_cast<DummyObject*>(
                receiver->getObject()
            );

        if (object == nullptr) return RuntimeValue(nullptr);

        object->value = value->get<int64_t>();

        return RuntimeValue(object->value);
    }
    RuntimeValue div(std::vector<RuntimeValue*>& args) {
        if (args.size() != 3)
            return RuntimeValue(nullptr);

        RuntimeValue* receiver = args[0];
        RuntimeValue* divisor = args[1];
        RuntimeValue* remainder = args[2];

        if (receiver == nullptr || divisor == nullptr || remainder == nullptr)
            return RuntimeValue(nullptr);

        if (!receiver->isObject() ||
            !divisor->is<int64_t>() ||
            !remainder->is<int64_t>())
            return RuntimeValue(nullptr);

        DummyObject* object =
            dynamic_cast<DummyObject*>(receiver->getObject());

        if (object == nullptr)
            return RuntimeValue(nullptr);

        int64_t divisorValue = divisor->get<int64_t>();

        if (divisorValue == 0)
            return RuntimeValue(nullptr);

        int64_t quotient = object->value / divisorValue;
        int64_t rest = object->value % divisorValue;

        object->value = quotient;
        *remainder = RuntimeValue(rest);

        return RuntimeValue(quotient);
    }

}

Library dummyLibrary
{
    "dummy",
    {
        {"hello", BuiltinEntry{dummy::hello}},
        {"create", BuiltinEntry{dummy::create}},
        //{"id", BuiltinEntry{dummy::id}}
    },
    {},
    dummy::createInitializer
};