#include "nokiClass.hpp"

#include "runtimeobjectManager.hpp"
#include "environment.hpp"

#include <stdexcept>
#include <string_view>
#include <utility>

namespace
{
    void ownASTStringViews(
        ASTNode& node,
        std::deque<std::string>& storage
    )
    {
        if (std::holds_alternative<std::string_view>(node.as))
        {
            const std::string_view original =
                std::get<std::string_view>(node.as);

            storage.emplace_back(original);

            node.as = std::string_view(storage.back());
        }

        for (auto& child : node.children)
        {
            if (child != nullptr)
            {
                ownASTStringViews(*child, storage);
            }
        }
    }

    std::size_t countNestedClassDeclarations(
        const ASTNode& declaration
    )
    {
        std::size_t count = 0;

        for (const auto& member : declaration.children)
        {
            if (
                member == nullptr ||
                member->getType() !=
                    ASTNode::TypeNode::CLASS_DECLARATION
            )
            {
                continue;
            }

            ++count;
            count += countNestedClassDeclarations(*member);
        }

        return count;
    }
}

NokiClass::NokiClass(
    RuntimeObjectManager* manager,
    RuntimeObjectTypeID objectTypeID,
    std::string moduleName,
    std::string className,
    std::string qualifiedName,
    RuntimeObjectTypeID classTypeID,
    Environment* moduleEnvironment,
    std::unique_ptr<ASTNode> declaration,
    RuntimeObjectInstanceID parentDefinitionID
)
    : RuntimeObject(manager, objectTypeID),
      moduleName(std::move(moduleName)),
      className(std::move(className)),
      qualifiedName(std::move(qualifiedName)),
      classTypeID(classTypeID),
      moduleEnvironment(moduleEnvironment),
      parentDefinitionID(parentDefinitionID),
      declaration(std::move(declaration))
{
    if (
        manager == nullptr ||
        moduleEnvironment == nullptr ||
        !manager->exists(classTypeID)
    )
    {
        throw std::runtime_error(
            "Invalid Noki class runtime registration."
        );
    }

    if (
        this->declaration == nullptr ||
        this->declaration->getType() !=
            ASTNode::TypeNode::CLASS_DECLARATION ||
        this->declaration->children.empty() ||
        this->declaration->children[0] == nullptr ||
        this->declaration->children[0]->getType() !=
            ASTNode::TypeNode::VARIABLE
    )
    {
        throw std::runtime_error(
            "Invalid Noki class declaration AST."
        );
    }

    if (
        this->declaration->children[0]->getString() !=
        this->className
    )
    {
        throw std::runtime_error(
            "Noki class name does not match its declaration."
        );
    }

    if (this->qualifiedName.empty())
    {
        throw std::runtime_error(
            "Noki class identity cannot be empty."
        );
    }

    if (
        parentDefinitionID != INVALID_CANONICAL_ID &&
        manager->findInstance(parentDefinitionID) == nullptr
    )
    {
        throw std::runtime_error(
            "Nested Noki class has no registered parent definition."
        );
    }

    // Fazer a definição independente do tempo de vida do
    // texto-fonte, incluindo os textos dos membros aninhados.
    ownASTStringViews(
        *this->declaration,
        ownedStrings
    );
}

NokiClass::NokiClass(
    RuntimeObjectManager* manager,
    RuntimeObjectTypeID objectTypeID,
    std::string moduleName,
    std::string className,
    std::string qualifiedName,
    RuntimeObjectTypeID classTypeID,
    Environment* moduleEnvironment,
    RuntimeObjectInstanceID canonicalDefinitionID,
    RuntimeObjectInstanceID parentDefinitionID
)
    : RuntimeObject(manager, objectTypeID),
      moduleName(std::move(moduleName)),
      className(std::move(className)),
      qualifiedName(std::move(qualifiedName)),
      classTypeID(classTypeID),
      moduleEnvironment(moduleEnvironment),
      canonicalDefinitionID(canonicalDefinitionID),
      parentDefinitionID(parentDefinitionID)
{
    if (
        manager == nullptr ||
        moduleEnvironment == nullptr ||
        !manager->exists(classTypeID) ||
        canonicalDefinitionID == INVALID_CANONICAL_ID
    )
    {
        throw std::runtime_error(
            "Invalid Noki class definition reference."
        );
    }
}

void NokiClass::establishCanonicalIdentity()
{
    RuntimeObjectManager* manager = getObjectManager();

    if (
        manager == nullptr ||
        declaration == nullptr ||
        getInstanceID() == INVALID_CANONICAL_ID ||
        manager->findInstance(getInstanceID()) != this ||
        canonicalDefinitionID != INVALID_CANONICAL_ID
    )
    {
        throw std::runtime_error(
            "Cannot establish canonical Noki class identity."
        );
    }

    canonicalDefinitionID = getInstanceID();
}

bool NokiClass::isCanonical() const noexcept
{
    return
        declaration != nullptr &&
        canonicalDefinitionID != INVALID_CANONICAL_ID &&
        canonicalDefinitionID == getInstanceID();
}

NokiClass* NokiClass::resolveCanonicalDefinition()
{
    if (isCanonical())
    {
        return this;
    }

    RuntimeObjectManager* manager = getObjectManager();

    if (
        manager == nullptr ||
        canonicalDefinitionID == INVALID_CANONICAL_ID
    )
    {
        throw std::runtime_error(
            "Noki class has no valid canonical definition."
        );
    }

    auto* canonical =
        dynamic_cast<NokiClass*>(
            manager->findInstance(canonicalDefinitionID)
        );

    if (
        canonical == nullptr ||
        !canonical->isCanonical()
    )
    {
        throw std::runtime_error(
            "Canonical Noki class definition is unavailable."
        );
    }

    return canonical;
}

const NokiClass* NokiClass::resolveCanonicalDefinition() const
{
    if (isCanonical())
    {
        return this;
    }

    RuntimeObjectManager* manager = getObjectManager();

    if (
        manager == nullptr ||
        canonicalDefinitionID == INVALID_CANONICAL_ID
    )
    {
        throw std::runtime_error(
            "Noki class has no valid canonical definition."
        );
    }

    const auto* canonical =
        dynamic_cast<const NokiClass*>(
            manager->findInstance(canonicalDefinitionID)
        );

    if (
        canonical == nullptr ||
        !canonical->isCanonical()
    )
    {
        throw std::runtime_error(
            "Canonical Noki class definition is unavailable."
        );
    }

    return canonical;
}

const ASTNode& NokiClass::getDeclaration() const
{
    const NokiClass* canonical =
        resolveCanonicalDefinition();

    if (canonical->declaration == nullptr)
    {
        throw std::runtime_error(
            "Canonical Noki class has no AST."
        );
    }

    return *canonical->declaration;
}

std::unique_ptr<RuntimeObject> NokiClass::clone() const
{
    RuntimeObjectManager* manager = getObjectManager();

    if (
        manager == nullptr ||
        canonicalDefinitionID == INVALID_CANONICAL_ID
    )
    {
        throw std::runtime_error(
            "Cannot clone an unregistered Noki class."
        );
    }

    // Verificar que a definição original continua acessível.
    (void)getDeclaration();

    // Criar uma cópia registada que aponta para a mesma
    // definição canónica, sem copiar a AST.
    return manager->createObject<NokiClass>(
        getTypeID(),
        moduleName,
        className,
        qualifiedName,
        classTypeID,
        moduleEnvironment,
        canonicalDefinitionID,
        parentDefinitionID
    );
}

void NokiClass::addNestedClass(
    const std::string& name,
    RuntimeValue value
)
{
    if (!isCanonical())
    {
        throw std::runtime_error(
            "Nested classes can only be registered "
            "in a canonical Noki class."
        );
    }

    if (name.empty() || !value.isObject())
    {
        throw std::runtime_error(
            "Invalid nested Noki class value."
        );
    }

    auto* nestedClass =
        dynamic_cast<NokiClass*>(value.getObject());

    if (
        nestedClass == nullptr ||
        !nestedClass->isCanonical() ||
        nestedClass->getParentDefinitionID() !=
            getCanonicalDefinitionID() ||
        nestedClass->getClassName() != name ||
        nestedClass->getQualifiedName() !=
            qualifiedName + "::" + name
    )
    {
        throw std::runtime_error(
            "Nested Noki class has an invalid identity."
        );
    }

    const auto [it, inserted] =
        nestedClasses.emplace(name, std::move(value));

    if (!inserted)
    {
        throw std::runtime_error(
            "Duplicate nested Noki class: " + name
        );
    }
}

RuntimeValue* NokiClass::findNestedClass(
    const std::string& name
)
{
    NokiClass* canonical =
        resolveCanonicalDefinition();

    auto it = canonical->nestedClasses.find(name);

    if (it == canonical->nestedClasses.end())
    {
        return nullptr;
    }

    return &it->second;
}

const RuntimeValue* NokiClass::findNestedClass(
    const std::string& name
) const
{
    const NokiClass* canonical =
        resolveCanonicalDefinition();

    auto it = canonical->nestedClasses.find(name);

    if (it == canonical->nestedClasses.end())
    {
        return nullptr;
    }

    return &it->second;
}

void NokiClass::registerNestedClasses(
    RuntimeObjectManager& manager,
    RuntimeObjectTypeID metaTypeID,
    std::vector<RuntimeObjectTypeID>& registeredTypeIDs
)
{
    if (!isCanonical())
    {
        throw std::runtime_error(
            "Nested classes can only be registered "
            "from a canonical class definition."
        );
    }

    if (!manager.exists(metaTypeID))
    {
        throw std::runtime_error(
            "Noki class meta-type is not registered."
        );
    }

    const ASTNode& classDeclaration =
        getDeclaration();

    // Reservar memória para todos os IDs antes de começar
    // a registar os tipos dos descendentes.
    const std::size_t nestedCount =
        countNestedClassDeclarations(classDeclaration);

    if (
        nestedCount >
        registeredTypeIDs.max_size() -
            registeredTypeIDs.size()
    )
    {
        throw std::length_error(
            "Too many nested Noki class declarations."
        );
    }

    registeredTypeIDs.reserve(
        registeredTypeIDs.size() + nestedCount
    );

    for (const auto& member : classDeclaration.children)
    {
        if (
            member == nullptr ||
            member->getType() !=
                ASTNode::TypeNode::CLASS_DECLARATION
        )
        {
            continue;
        }

        if (
            member->children.empty() ||
            member->children[0] == nullptr ||
            member->children[0]->getType() !=
                ASTNode::TypeNode::VARIABLE
        )
        {
            throw std::runtime_error(
                "Invalid nested Noki class declaration."
            );
        }

        const std::string nestedName(
            member->children[0]->getString()
        );

        const std::string nestedQualifiedName =
            qualifiedName + "::" + nestedName;

        if (manager.exists(nestedQualifiedName))
        {
            throw std::runtime_error(
                "Noki nested class type is already registered: " +
                nestedQualifiedName
            );
        }

        auto nestedDeclaration =
            ASTNode::ast_clone(*member);

        if (nestedDeclaration == nullptr)
        {
            throw std::runtime_error(
                "Failed to clone nested Noki class declaration."
            );
        }

        const RuntimeObjectTypeID nestedTypeID =
            manager.registerType(nestedQualifiedName);

        // O vector já tem capacidade reservada.
        registeredTypeIDs.push_back(nestedTypeID);

        // O ambiente conserva apenas os IDs para rollback,
        // não definições de classes.
        moduleEnvironment->registerNokiClassTypeID(
            nestedTypeID
        );

        auto nestedObject =
            manager.createObject<NokiClass>(
                metaTypeID,
                moduleName,
                nestedName,
                nestedQualifiedName,
                nestedTypeID,
                moduleEnvironment,
                std::move(nestedDeclaration),
                getCanonicalDefinitionID()
            );

        nestedObject->establishCanonicalIdentity();

        NokiClass* nestedObjectPointer =
            nestedObject.get();

        // Transferir o ownership para o RuntimeValue guardado
        // na definição canónica da classe exterior.
        addNestedClass(
            nestedName,
            RuntimeValue::fromObject(
                std::move(nestedObject)
            )
        );

        // Registar recursivamente os descendentes.
        nestedObjectPointer->registerNestedClasses(
            manager,
            metaTypeID,
            registeredTypeIDs
        );
    }
}