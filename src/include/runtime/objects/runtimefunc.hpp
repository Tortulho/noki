#ifndef RUNTIMEFUNC
#define RUNTIMEFUNC

#include <string>
#include <utility>
#include <memory>
#include <vector>

#include "asttypes.hpp"

class ASTNode;

enum class FunctionParameterMode
{
    NORMAL,
    MUT,
    CONST
};

class FunctionParameter
{
public:

    FunctionParameter(
        std::string name,
        TypeNode type,
        FunctionParameterMode mode
    )
        : name(std::move(name)),
          type(type),
          mode(mode)
    {}

    const std::string& getName() const noexcept
    {
        return name;
    }

    TypeNode getType() const noexcept
    {
        return type;
    }

    FunctionParameterMode getMode() const noexcept
    {
        return mode;
    }

    bool isConst() const noexcept
    {
        return mode == FunctionParameterMode::CONST;
    }

    bool isMut() const noexcept
    {
        return mode == FunctionParameterMode::MUT;
    }

private:

    std::string name;
    TypeNode type;
    FunctionParameterMode mode;
};

class NokiFunction
{
public:

    NokiFunction(
        std::string name,
        std::vector<FunctionParameter> parameters,
        std::unique_ptr<ASTNode> body
    );

    ~NokiFunction();

    const std::string& getName() const noexcept;

    const std::vector<FunctionParameter>&
    getParameters() const noexcept;

    const ASTNode& getBody() const noexcept;

    NokiFunction(const NokiFunction&) = delete;
    NokiFunction& operator=(const NokiFunction&) = delete;

    NokiFunction(NokiFunction&&) noexcept = default;
    NokiFunction& operator=(NokiFunction&&) noexcept = default;

private:

    std::string name;
    std::vector<FunctionParameter> parameters;
    std::unique_ptr<ASTNode> body;
};

#endif