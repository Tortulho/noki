#ifndef ENVIRO
#define ENVIRO

#include <memory>
#include <unordered_map>
#include <string>

#include "runtimeval.hpp"
#include "runtimefunc.hpp"

class Environment
{
    public:
        explicit Environment(Environment* parent = nullptr)
        : parent(parent) {}

        RuntimeValue* findVar(const std::string& name);
        RuntimeValue* findLocalVar(const std::string& name);

        RuntimeValue& getVar(const std::string& name);

        void createVar(const std::string& name, RuntimeValue value);
        void createConst(const std::string& name, RuntimeValue value);
        void createReference(
            const std::string& name,
            RuntimeValue* value
        );

        bool assignVar(const std::string& name, RuntimeValue value);
        bool removeVar(const std::string& name);

        bool existLocal(const std::string& name) const;
        bool exist(const std::string& name) const;

        inline Environment* getParent() const {
            return parent;
        }

        NokiFunction* findNokiFunction(const std::string& name);
        const NokiFunction* findNokiFunction(const std::string& name) const;

        void createNokiFunction(
            const std::string& name,
            std::unique_ptr<NokiFunction> function
        );

        bool existLocalNokiFunction(
            const std::string& name
        ) const
        {
            return nokiFunctions.find(name) != nokiFunctions.end();
        }

    private:
        std::unordered_map<std::string, RuntimeValue> variables;

        std::unordered_map<std::string, RuntimeValue> constValues;

        // Non-owning references used by mutable parameters.
        std::unordered_map<std::string, RuntimeValue*> references;

        Environment* parent = nullptr;

        std::unordered_map<
            std::string, std::unique_ptr<NokiFunction>
        > nokiFunctions;
};

#endif