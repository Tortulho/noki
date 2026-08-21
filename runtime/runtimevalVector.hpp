#ifndef RT_VEC
#define RT_VEC

#include <vector>
#include "runtimeval.hpp"
#include <utility>

class RuntimeValue_Vector {

    private:
        std::vector<RuntimeValue> values;

    public:

        RuntimeValue_Vector() = default;

        RuntimeValue_Vector(const RuntimeValue_Vector& other)
        : values(other.values) {}

        RuntimeValue_Vector& operator=(const RuntimeValue_Vector& other)
        {
            if (this != &other) values = other.values;
            return *this;
        }

        RuntimeValue_Vector(RuntimeValue_Vector&& other) noexcept = default;

        RuntimeValue_Vector& operator=(RuntimeValue_Vector&& other) noexcept = default;

        ~RuntimeValue_Vector() = default;

        //FUNCS

        size_t size() const {
            return values.size();
        }

        RuntimeValue& at(size_t index) {
            if (index < 0) index = values.size()+index;
            if (index > values.size())
                throw std::out_of_range("Vector index out of bounds");
            return values.at(index);
        }
        const RuntimeValue& at(size_t index) const {
            if (index < 0) index = values.size()+index;
            if (index > values.size())
                throw std::out_of_range("Vector index out of bounds");
            return values.at(index);
        }

        void pushback(RuntimeValue value) {
            values.push_back(std::move(value));
        }

        void insert(size_t index, RuntimeValue value) {
            if (index < 0) index = values.size()+index;
            if (index > values.size())
                throw std::out_of_range("Vector index out of bounds");

            values.insert(
                values.begin() + index,
                std::move(value)
            );
        }

        void pop(size_t index) {
            if (index < 0) index = values.size()+index;
            if (index > values.size())
                throw std::out_of_range("Vector index out of bounds");

            values.erase(values.begin() + index);
        }

        size_t remove(
            size_t count,
            const RuntimeValue& item)
        {
            if (count == 0) return 0;
            if (count == -1) count = INT64_MAX;

            size_t removed = 0;

            for (size_t i = 0; i < values.size() && removed < count;)
            {
                if (RuntimeValue::comp(
                        values[i],
                        item,
                        ASTNode::EQUAL))
                {
                    values.erase(values.begin() + i);
                    removed++;
                }
                else
                {
                    i++;
                }
            }

            return removed;
        }

        int64_t find(
            const RuntimeValue& item) const
        {
            for (size_t i = 0; i < values.size(); i++)
            {
                if (RuntimeValue::comp(
                        values[i],
                        item,
                        ASTNode::EQUAL))
                {
                    return static_cast<int64_t>(i);
                }
            }

            return -1;
        }

        


};

#endif