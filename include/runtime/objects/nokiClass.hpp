#ifndef NOKICLASS
#define NOKICLASS

#include <deque>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "runtimeobject.hpp"
#include "runtimeval.hpp"

class Environment;
class RuntimeObjectManager;

class NokiClass final : public RuntimeObject
{
public:

    static constexpr RuntimeObjectInstanceID INVALID_CANONICAL_ID =
        std::numeric_limits<RuntimeObjectInstanceID>::max();

    // Definição canónica de uma classe.
    NokiClass(
        RuntimeObjectManager* manager,
        RuntimeObjectTypeID objectTypeID,
        std::string moduleName,
        std::string className,
        std::string qualifiedName,
        RuntimeObjectTypeID classTypeID,
        Environment* moduleEnvironment,
        std::unique_ptr<ASTNode> declaration,
        RuntimeObjectInstanceID parentDefinitionID =
            INVALID_CANONICAL_ID
    );

    // Cópia registada que referencia a definição canónica.
    NokiClass(
        RuntimeObjectManager* manager,
        RuntimeObjectTypeID objectTypeID,
        std::string moduleName,
        std::string className,
        std::string qualifiedName,
        RuntimeObjectTypeID classTypeID,
        Environment* moduleEnvironment,
        RuntimeObjectInstanceID canonicalDefinitionID,
        RuntimeObjectInstanceID parentDefinitionID =
            INVALID_CANONICAL_ID
    );

    std::unique_ptr<RuntimeObject> clone() const override;

    void establishCanonicalIdentity();

    bool isCanonical() const noexcept;

    const ASTNode& getDeclaration() const;

    const std::string& getModuleName() const noexcept
    {
        return moduleName;
    }

    const std::string& getClassName() const noexcept
    {
        return className;
    }

    const std::string& getQualifiedName() const noexcept
    {
        return qualifiedName;
    }

    RuntimeObjectTypeID getClassTypeID() const noexcept
    {
        return classTypeID;
    }

    RuntimeObjectInstanceID getCanonicalDefinitionID() const noexcept
    {
        return canonicalDefinitionID;
    }

    RuntimeObjectInstanceID getParentDefinitionID() const noexcept
    {
        return parentDefinitionID;
    }

    Environment* getModuleEnvironment() const noexcept
    {
        return moduleEnvironment;
    }

    // Regista recursivamente as classes aninhadas
    // na definição canónica atual.
    void registerNestedClasses(
        RuntimeObjectManager& manager,
        RuntimeObjectTypeID metaTypeID,
        std::vector<RuntimeObjectTypeID>& registeredTypeIDs
    );

    // As classes aninhadas são armazenadas como RuntimeValue.
    void addNestedClass(
        const std::string& name,
        RuntimeValue value
    );

    RuntimeValue* findNestedClass(
        const std::string& name
    );

    const RuntimeValue* findNestedClass(
        const std::string& name
    ) const;

private:

    NokiClass* resolveCanonicalDefinition();

    const NokiClass* resolveCanonicalDefinition() const;

    std::string moduleName;
    std::string className;
    std::string qualifiedName;

    RuntimeObjectTypeID classTypeID;

    // Não proprietário: o módulo é dono do seu Environment.
    Environment* moduleEnvironment;

    RuntimeObjectInstanceID canonicalDefinitionID =
        INVALID_CANONICAL_ID;

    RuntimeObjectInstanceID parentDefinitionID =
        INVALID_CANONICAL_ID;

    // A AST contém string_views. Esta storage mantém os seus
    // conteúdos válidos durante toda a vida da definição.
    std::deque<std::string> ownedStrings;

    // Apenas a definição canónica possui a AST.
    std::unique_ptr<ASTNode> declaration;

    // Contém os RuntimeValue das definições aninhadas.
    // A definição canónica exterior é a proprietária.
    std::unordered_map<std::string, RuntimeValue> nestedClasses;
};

#endif