#ifndef RUNTIMEVAL
#define RUNTIMEVAL

#include <string>
#include <memory>
#include <variant>
#include <iostream>
#include "runtimeobject.hpp"
#include "runtimefunc.hpp"
#include "ast.hpp"
#include "runtimevalTypeID.hpp"

class RuntimeValue_Vector;

using RTValue = std::variant<
    int64_t,
    double,
    bool,
    void*, //for null
    std::string,
    std::unique_ptr<RuntimeObject>,
    std::unique_ptr<RuntimeValue_Vector>
>;

class RuntimeValue {

    private:
        RTValue value;

    public:

        RuntimeValue()
        : value(nullptr) {}
        RuntimeValue(RTValue value)
        : value(std::move(value)) {}
        RuntimeValue(void* ptr) 
        : value(ptr) {}

        RuntimeValue(const RuntimeValue& other);
        RuntimeValue& operator=(const RuntimeValue& other);

        RuntimeValue(RuntimeValue&& other) noexcept;
        RuntimeValue& operator=(RuntimeValue&& other) noexcept;

        ~RuntimeValue();
        
        static RuntimeValue fromObject(std::unique_ptr<RuntimeObject> object);

        void printValue(const RTValue& value);

        void print() const;

        void println() const;

        template<typename T>
        T& get()
        {
            return std::get<T>(value);
        }
        template<typename T>
        const T& get() const
        {
            return std::get<T>(value);
        }

        template<typename T>
        bool is() const
        {
            return std::holds_alternative<T>(value);
        }

        bool isObject() const
        {
            return std::holds_alternative<std::unique_ptr<RuntimeObject>>(value);
        }

        RuntimeObject* getObject()
        {
            auto& object =
                std::get<std::unique_ptr<RuntimeObject>>(value);

            return object.get();
        }

        const RuntimeObject* getObject() const
        {
            const auto& object =
                std::get<std::unique_ptr<RuntimeObject>>(value);

            return object.get();
        }

        RuntimeObjectTypeID getObjectType() const
        {
            const RuntimeObject* object = getObject();

            if (object == nullptr)
                throw std::runtime_error(
                    "RuntimeValue does not contain a RuntimeObject."
                );

            return object->getTypeID();
        }


        static bool runtimeValueToBool(const RuntimeValue& value);

        //VECTOR RELATED FUNCS
        bool isVector() const
        {
            return std::holds_alternative<
                std::unique_ptr<RuntimeValue_Vector>
            >(value);
        }

        RuntimeValue_Vector* getVector()
        {
            auto& vector =
                std::get<std::unique_ptr<RuntimeValue_Vector>>(value);

            return vector.get();
        }

        const RuntimeValue_Vector* getVector() const
        {
            const auto& vector =
                std::get<std::unique_ptr<RuntimeValue_Vector>>(value);

            return vector.get();
        }

        //COMPARISONS

        static bool comp(
            const RuntimeValue& left,
            const RuntimeValue& right,
            ASTNode::TypeOp op
        );

        RuntimeValueTypeID getTypeID() const
        {
            if (is<int64_t>())
                return RuntimeValueTypeID::INT;

            if (is<double>())
                return RuntimeValueTypeID::FLOAT;

            if (is<bool>())
                return RuntimeValueTypeID::BOOL;

            if (is<std::string>())
                return RuntimeValueTypeID::STRING;

            if (isVector())
                return RuntimeValueTypeID::VECTOR;

            if (is<void*>())
                return RuntimeValueTypeID::NULL_VALUE;

            throw std::runtime_error(
                "RuntimeValue does not contain an intrinsic value type."
            );
        }


};

#include "runtimevalVector.hpp"

#endif