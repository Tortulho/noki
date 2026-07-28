#include <unordered_map>
#include <string>
#include "../runtime/runtimeval.hpp"

class Environment
{
public:
    std::unordered_map<std::string, RuntimeValue> variables;
    Environment* parent = nullptr;

    explicit Environment(Environment* parent = nullptr)
        : parent(parent)
    {}

    RuntimeValue* findVar(const std::string& name);
    RuntimeValue& getVar(const std::string& name);
    void createVar(const std::string& name, RuntimeValue value);
    bool existLocal(const std::string& name) const;
    bool exist(const std::string& name) const;
    bool assignVar(const std::string& name, RuntimeValue value);
};