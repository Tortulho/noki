#include "nokiModule.hpp"
#include "ast.hpp"
#include "environment.hpp"

NokiModule::NokiModule(
    std::string name,
    std::filesystem::path path,
    std::string source,
    std::unique_ptr<ASTNode> ast,
    std::unique_ptr<Environment> environment
)
    : name(std::move(name)),
      path(std::move(path)),
      source(std::move(source)),
      ast(std::move(ast)),
      environment(std::move(environment))
{
}

NokiModule::~NokiModule() = default;

NokiModule::NokiModule(NokiModule&&) noexcept = default;

NokiModule& NokiModule::operator=(NokiModule&&) noexcept = default;

const std::string& NokiModule::getName() const noexcept
{
    return name;
}

const std::filesystem::path& NokiModule::getPath() const noexcept
{
    return path;
}

ASTNode* NokiModule::getAST() noexcept
{
    return ast.get();
}

const ASTNode* NokiModule::getAST() const noexcept
{
    return ast.get();
}

Environment* NokiModule::getEnvironment() noexcept
{
    return environment.get();
}

const Environment* NokiModule::getEnvironment() const noexcept
{
    return environment.get();
}