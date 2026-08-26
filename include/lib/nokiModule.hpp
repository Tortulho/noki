#ifndef NOKIMODULE_HPP
#define NOKIMODULE_HPP

#include <filesystem>
#include <memory>
#include <string>

class ASTNode;
class Environment;

class NokiModule
{
public:

    NokiModule(
        std::string name,
        std::filesystem::path path,
        std::string source,
        std::unique_ptr<ASTNode> ast,
        std::unique_ptr<Environment> environment
    );

    ~NokiModule();

    NokiModule(const NokiModule&) = delete;
    NokiModule& operator=(const NokiModule&) = delete;

    NokiModule(NokiModule&&) noexcept;
    NokiModule& operator=(NokiModule&&) noexcept;

    const std::string& getName() const noexcept;

    const std::filesystem::path& getPath() const noexcept;

    ASTNode* getAST() noexcept;
    const ASTNode* getAST() const noexcept;

    Environment* getEnvironment() noexcept;
    const Environment* getEnvironment() const noexcept;

private:

    std::string source;
    std::string name;
    std::filesystem::path path;
    std::unique_ptr<ASTNode> ast;
    std::unique_ptr<Environment> environment;
};

#endif