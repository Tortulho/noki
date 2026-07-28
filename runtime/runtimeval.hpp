#ifndef RUNTIMEVAL
#define RUNTIMEVAL

#include <string>
#include <memory>
#include <variant>
#include <iostream>

using RTValue = std::variant<
    int64_t,
    double,
    bool,
    void*,
    std::string
>;

class RuntimeValue {

    public:
        // enum Type {
        //     INT,
        //     FLOAT,
        //     BOOL,
        //     POINTER,
        //     STRING,
        //     null,
        // };

    private:
        RTValue value;
        //Type type;

    public:

        RuntimeValue()
        : value(nullptr) {}
        RuntimeValue(RTValue value)
        : value(std::move(value)) {}
        RuntimeValue(void* ptr) 
        : value(ptr) {}
        

        void printValue(const RTValue& value) {
            std::visit([](auto&& v) {

                std::cout << v << "\n";

            }, value);
        }

        void print() const
        {
            std::visit([](auto&& v)
            {
                using T = std::decay_t<decltype(v)>;

                if constexpr(std::is_same_v<T, bool>)
                {
                    std::cout << (v ? "true" : "false");
                }
                else if constexpr(std::is_same_v<T, void*>)
                {
                    if(v == nullptr)
                        std::cout << "null";
                    else
                        std::cout << v;
                }
                else
                {
                    std::cout << v;
                }

                std::cout << "\n";                

            }, value);
        }

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
        /*
        RuntimeValue x(int64_t(50));
        std::cout << x.get<int64_t>();
        */
        template<typename T>
        bool is() const
        {
            return std::holds_alternative<T>(value);
        }

};

#endif