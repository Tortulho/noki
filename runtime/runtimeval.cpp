#include "runtimeval.hpp"
#include "runtimevalVector.hpp"
#include "../ast.hpp"

bool RuntimeValue::runtimeValueToBool(const RuntimeValue& value)
{
    if (value.is<bool>())
        return value.get<bool>();

    if (value.is<int64_t>())
        return value.get<int64_t>() != 0;

    if (value.is<double>())
        return value.get<double>() != 0.0;

    return false;
}

RuntimeValue::RuntimeValue(const RuntimeValue& other)
{
    std::visit(
        [this](const auto& otherValue) -> void
        {
            using T = std::decay_t<decltype(otherValue)>;

            if constexpr (
                std::is_same_v<
                    T,
                    std::unique_ptr<RuntimeObject>
                >
            )
            {
                if (otherValue == nullptr)
                {
                    value = std::unique_ptr<RuntimeObject>();
                }
                else
                {
                    //TODO
                    if (otherValue == nullptr)
                    {
                        value = std::unique_ptr<RuntimeObject>();
                    }
                    else
                    {
                        value = otherValue->clone();
                    }
                }
            }
            else if constexpr (
                std::is_same_v<
                    T,
                    std::unique_ptr<RuntimeValue_Vector>
                >
            )
            {
                if (otherValue == nullptr)
                {
                    value = std::unique_ptr<RuntimeValue_Vector>();
                }
                else
                {
                    value = std::make_unique<RuntimeValue_Vector>(
                        *otherValue
                    );
                }
            }
            else
            {
                value = otherValue;
            }
        },
        other.value
    );
}

RuntimeValue& RuntimeValue::operator=(
    const RuntimeValue& other
)
{
    if (this == &other)
        return *this;

    std::visit(
        [this](const auto& otherValue) -> void
        {
            using T = std::decay_t<decltype(otherValue)>;

            if constexpr (
                std::is_same_v<
                    T,
                    std::unique_ptr<RuntimeObject>
                >
            )
            {
                if (otherValue == nullptr)
                {
                    value = std::unique_ptr<RuntimeObject>();
                }
                else
                {
                    //TODO
                    if (otherValue == nullptr)
                    {
                        value = std::unique_ptr<RuntimeObject>();
                    }
                    else
                    {
                        value = otherValue->clone();
                    }
                }
            }
            else if constexpr (
                std::is_same_v<
                    T,
                    std::unique_ptr<RuntimeValue_Vector>
                >
            )
            {
                if (otherValue == nullptr)
                {
                    value = std::unique_ptr<RuntimeValue_Vector>();
                }
                else
                {
                    value = std::make_unique<RuntimeValue_Vector>(
                        *otherValue
                    );
                }
            }
            else
            {
                value = otherValue;
            }
        },
        other.value
    );

    return *this;
}

RuntimeValue::RuntimeValue(RuntimeValue&& other) noexcept
    : value(std::move(other.value)) {}

RuntimeValue& RuntimeValue::operator=(RuntimeValue&& other) noexcept
{
    if (this == &other) return *this;

    value = std::move(other.value);

    return *this;
}

RuntimeValue::~RuntimeValue() = default;

RuntimeValue RuntimeValue::fromObject(
    std::unique_ptr<RuntimeObject> object)
{
    return RuntimeValue(
        RTValue(std::move(object))
    );
}

// PRINTS

void RuntimeValue::printValue(const RTValue& value)
{
    std::visit([](auto&& v)
    {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, bool>)
        {
            std::cout << (v ? "true" : "false");
        }
        else if constexpr (std::is_same_v<T, void*>)
        {
            if (v == nullptr)
                std::cout << "null";
            else
                std::cout << v;
        }
        else if constexpr (
            std::is_same_v<T, std::unique_ptr<RuntimeObject>>
        )
        {
            if (v == nullptr)
                std::cout << "null";
            else
                std::cout << "<object>";
        }
        else if constexpr (
            std::is_same_v<T, std::unique_ptr<RuntimeValue_Vector>>
        )
        {
            if (v == nullptr)
            {
                std::cout << "null";
            }
            else
            {
                std::cout << "[";

                for (size_t i = 0; i < v->size(); ++i)
                {
                    if (i > 0)
                        std::cout << ", ";

                    v->at(i).print();
                }

                std::cout << "]";
            }
        }
        else
        {
            std::cout << v;
        }

    }, value);

    std::cout << '\n';
}

void RuntimeValue::print() const
{
    std::visit([](auto&& v)
    {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, bool>)
        {
            std::cout << (v ? "true" : "false");
        }
        else if constexpr (std::is_same_v<T, void*>)
        {
            if (v == nullptr)
                std::cout << "null";
            else
                std::cout << v;
        }
        else if constexpr (
            std::is_same_v<T, std::unique_ptr<RuntimeObject>>
        )
        {
            if (v == nullptr)
                std::cout << "null";
            else
                std::cout << "<object>";
        }
        else if constexpr (
            std::is_same_v<T, std::unique_ptr<RuntimeValue_Vector>>
        )
        {
            if (v == nullptr)
            {
                std::cout << "null";
            }
            else
            {
                std::cout << "[";

                for (size_t i = 0; i < v->size(); ++i)
                {
                    if (i > 0)
                        std::cout << ", ";

                    v->at(i).print();
                }

                std::cout << "]";
            }
        }
        else
        {
            std::cout << v;
        }

    }, value);
}

void RuntimeValue::println() const
{
    print();
    std::cout << '\n';
}

bool RuntimeValue::comp(
    const RuntimeValue& left,
    const RuntimeValue& right,
    ASTNode::TypeOp op)
{

    switch (op)
    {
        case ASTNode::EQUAL:
            if (left.is<int64_t>() && right.is<int64_t>()) {
                return ((bool)(left.get<int64_t>() == right.get<int64_t>()));
            }
            if (left.is<double>() && right.is<double>()) {
                return ((bool)(left.get<double>() == right.get<double>()));
            }
            if (left.is<bool>() && right.is<bool>()) {
                return ((bool)(left.get<bool>() == right.get<bool>()));
            }
            if (left.is<int64_t>() && right.is<double>()) {
                return ((bool)((double)left.get<int64_t>() == right.get<double>()));
            }
            if (left.is<double>() && right.is<int64_t>()) {
                return ((bool)(left.get<double>() == (double)right.get<int64_t>()));
            }
            if (left.is<std::string>() && right.is<std::string>()) {
                return ((bool)(left.get<std::string>() == right.get<std::string>()));
            }
            if (left.isVector() && right.isVector()) {
                const RuntimeValue_Vector* leftVector = left.getVector();
                const RuntimeValue_Vector* rightVector = right.getVector();
                if (leftVector == nullptr || rightVector == nullptr)
                    return leftVector == rightVector;
            }
            return ((bool)false);
        case ASTNode::NOT_EQUAL:
            if (left.is<int64_t>() && right.is<int64_t>()) {
                return ((bool)(left.get<int64_t>() != right.get<int64_t>()));
            }
            if (left.is<double>() && right.is<double>()) {
                return ((bool)(left.get<double>() != right.get<double>()));
            }
            if (left.is<bool>() && right.is<bool>()) {
                return ((bool)(left.get<bool>() != right.get<bool>()));
            }
            if (left.is<int64_t>() && right.is<double>()) {
                return ((bool)((double)left.get<int64_t>() != right.get<double>()));
            }
            if (left.is<double>() && right.is<int64_t>()) {
                return ((bool)(left.get<double>() != (double)right.get<int64_t>()));
            }
            if (left.is<std::string>() && right.is<std::string>()) {
                return ((bool)(left.get<std::string>() != right.get<std::string>()));
            }
            if (left.isVector() && right.isVector()) {
                const RuntimeValue_Vector* leftVector = left.getVector();
                const RuntimeValue_Vector* rightVector = right.getVector();
                if (leftVector == nullptr || rightVector == nullptr)
                    return leftVector != rightVector;
            }
            return ((bool)true);
        case ASTNode::LESS:
            if (left.is<int64_t>() && right.is<int64_t>()) {
                return ((bool)(left.get<int64_t>() < right.get<int64_t>()));
            }
            if (left.is<double>() && right.is<double>()) {
                return ((bool)(left.get<double>() < right.get<double>()));
            }
            if (left.is<bool>() && right.is<bool>()) {
                return ((bool)(left.get<bool>() < right.get<bool>()));
            }
            if (left.is<int64_t>() && right.is<double>()) {
                return ((bool)((double)left.get<int64_t>() < right.get<double>()));
            }
            if (left.is<double>() && right.is<int64_t>()) {
                return ((bool)(left.get<double>() < (double)right.get<int64_t>()));
            }
            if (left.is<std::string>() && right.is<std::string>()) {
                return ((bool)(left.get<std::string>().size() < right.get<std::string>().size()));
            }
            return ((bool)false);

        case ASTNode::LESS_EQUAL:
            if (left.is<int64_t>() && right.is<int64_t>()) {
                return ((bool)(left.get<int64_t>() <= right.get<int64_t>()));
            }
            if (left.is<double>() && right.is<double>()) {
                return ((bool)(left.get<double>() <= right.get<double>()));
            }
            if (left.is<bool>() && right.is<bool>()) {
                return ((bool)(left.get<bool>() <= right.get<bool>()));
            }
            if (left.is<int64_t>() && right.is<double>()) {
                return ((bool)((double)left.get<int64_t>() <= right.get<double>()));
            }
            if (left.is<double>() && right.is<int64_t>()) {
                return ((bool)(left.get<double>() <= (double)right.get<int64_t>()));
            }
            if (left.is<std::string>() && right.is<std::string>()) {
                return ((bool)(left.get<std::string>().size() <= right.get<std::string>().size()));
            }
            return ((bool)false);
        case ASTNode::GREATER:
            if (left.is<int64_t>() && right.is<int64_t>()) {
                return ((bool)(left.get<int64_t>() > right.get<int64_t>()));
            }
            if (left.is<double>() && right.is<double>()) {
                return ((bool)(left.get<double>() > right.get<double>()));
            }
            if (left.is<bool>() && right.is<bool>()) {
                return ((bool)(left.get<bool>() > right.get<bool>()));
            }
            if (left.is<int64_t>() && right.is<double>()) {
                return ((bool)((double)left.get<int64_t>() > right.get<double>()));
            }
            if (left.is<double>() && right.is<int64_t>()) {
                return ((bool)(left.get<double>() > (double)right.get<int64_t>()));
            }
            if (left.is<std::string>() && right.is<std::string>()) {
                return ((bool)(left.get<std::string>().size() > right.get<std::string>().size()));
            }
            return ((bool)false);
        case ASTNode::GREATER_EQUAL:
            if (left.is<int64_t>() && right.is<int64_t>()) {
                return ((bool)(left.get<int64_t>() >= right.get<int64_t>()));
            }
            if (left.is<double>() && right.is<double>()) {
                return ((bool)(left.get<double>() >= right.get<double>()));
            }
            if (left.is<bool>() && right.is<bool>()) {
                return ((bool)(left.get<bool>() >= right.get<bool>()));
            }
            if (left.is<int64_t>() && right.is<double>()) {
                return ((bool)((double)left.get<int64_t>() >= right.get<double>()));
            }
            if (left.is<double>() && right.is<int64_t>()) {
                return ((bool)(left.get<double>() >= (double)right.get<int64_t>()));
            }
            if (left.is<std::string>() && right.is<std::string>()) {
                return ((bool)(left.get<std::string>().size() >= right.get<std::string>().size()));
            }
            return ((bool)false);
    
        default:
            return ((bool)false);
    }


}