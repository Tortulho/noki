#include "runtimefunc.hpp"
#include "ast.hpp"

NokiFunction::NokiFunction(
    std::string name,
    std::vector<FunctionParameter> parameters,
    std::unique_ptr<ASTNode> body
)
    : name(std::move(name)),
      parameters(std::move(parameters)),
      body(std::move(body))
{}

NokiFunction::~NokiFunction() = default;

const std::string& NokiFunction::getName() const noexcept
{
    return name;
}

const std::vector<FunctionParameter>&
NokiFunction::getParameters() const noexcept
{
    return parameters;
}

const ASTNode& NokiFunction::getBody() const noexcept
{
    return *body;
}